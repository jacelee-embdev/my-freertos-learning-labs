// app_sys_orchestrator.c
#include "app_sys_orchestrator.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"
#include "led.h"
#include "beep.h"
#include "keyboard.h"
#include "oled.h"
#include "password.h"
#include "relay_lock.h"
#include "w25q128.h"
#include "uart1.h"

/* ---------------------------------------------------------------------------------- */
/* [配置参数区] 模块内部使用的宏定义与枚举                                            */
/* ---------------------------------------------------------------------------------- */

// 系统级消息载体
typedef struct {
    uint8_t cmd;
    uint8_t data;
} system_msg_t;

// 门禁业务逻辑状态机
enum {
    PWD_MODE_INPUT  = 0,
    PWD_MODE_MODIFY = 1,
    PWD_MODE_SET    = 2
};

// 触发源指令
#define MSG_CMD_KEY_INPUT                   0x01

// UI 渲染指令集
#define MSG_CMD_OLED_SHOW_SET_PWD           0x01    
#define MSG_CMD_OLED_SHOW_INPUT             0x02    
#define MSG_CMD_OLED_SHOW_RIGHT             0x03    
#define MSG_CMD_OLED_SHOW_WRONG             0x04    
#define MSG_CMD_OLED_SHOW_INPUT_NEW         0x05    
#define MSG_CMD_OLED_SHOW_INPUT_OLD         0x06    
#define MSG_CMD_OLED_SHOW_SUCCESS           0x07    
#define MSG_CMD_OLED_SHOW_CHAR              0x08    

// 系统级事件标志位 (跨任务状态同步)
#define SYS_EVT_PWD_CORRECT                 (1 << 0)    
#define SYS_EVT_PWD_WRONG                   (1 << 1)    
#define SYS_EVT_PWD_ALARM                   (1 << 2)    

// 听觉反馈指令集
#define MSG_CMD_BEEP_SHORT                  0x01    
#define MSG_CMD_BEEP_LONG                   0x02    

// 物理执行器指令集
#define MSG_CMD_LOCK_OPEN                   0x01
#define MSG_CMD_LOCK_CLOSE                  0x02

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄与全局变量                              */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t         keyboard_task_handle        = NULL;     // 硬件输入层：键盘扫描任务
static TaskHandle_t         password_task_handle        = NULL;     // 核心逻辑层：密码处理与状态机任务
static TaskHandle_t         display_task_handle         = NULL;     // 渲染展示层：屏幕刷新任务
static TaskHandle_t         monitor_task_handle         = NULL;     // 异常监控层：事件总线监听任务
static TaskHandle_t         beep_task_handle            = NULL;     // 异步反馈层：蜂鸣器控制任务
static TaskHandle_t         relay_lock_task_handle      = NULL;     // 物理执行层：继电器门锁控制任务

static QueueHandle_t        password_queue              = NULL;     // 传递按键事件的队列
static QueueHandle_t        display_queue               = NULL;     // 传递 UI 刷新指令的队列
static QueueHandle_t        beep_queue                  = NULL;     // 传递发声指令的队列
static QueueHandle_t        relay_lock_queue            = NULL;     // 传递开闭锁指令的队列

static TimerHandle_t        auto_lock_timer             = NULL;     // 开门后自动落锁的软件定时器
static SemaphoreHandle_t    w25q128_mutex               = NULL;     // SPI Flash 底层防冲突互斥锁
static EventGroupHandle_t   system_event_group          = NULL;     // 全局安防事件总线

static uint8_t              pwd_input_index             = 0;        // 内存态：当前密码输入位偏移量
static uint8_t              pwd_error_attempts          = 0;        // 内存态：连续鉴权失败计数器
static uint8_t              door_unlocked_status        = TRUE;     // 物理态：门锁当前开闭情况


/* ---------------------------------------------------------------------------------- */
/* [底层驱动层] 队列通信与基础数据收发                                                */
/* ---------------------------------------------------------------------------------- */

// 统一下发系统级消息至指定队列，隔离 FreeRTOS 原生 API
static void sys_send_msg_to_queue(QueueHandle_t queue, uint8_t cmd, uint8_t data)
{
    system_msg_t recv_msg;
    
    recv_msg.cmd = cmd;
    recv_msg.data = data;
    
    xQueueSend(queue, &recv_msg, portMAX_DELAY);
}

/* ---------------------------------------------------------------------------------- */
/* [系统回调层] 硬件中断或软件定时器回调                                              */
/* ---------------------------------------------------------------------------------- */

// 自动落锁守护回调 (由 FreeRTOS Daemon 定时器服务执行)
void sys_auto_lock_timer_cb(TimerHandle_t xTimer)
{
    if (door_unlocked_status == TRUE)
    {
        sys_send_msg_to_queue(relay_lock_queue, MSG_CMD_LOCK_CLOSE, 0);
        sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT, 0);
        door_unlocked_status = FALSE;
    }
}

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 业务流转实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 硬件输入层：高频轮询物理矩阵键盘状态，并将有效键值压入处理队列
static void sys_keyboard_scan_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_val;

    while (1)
    {
        key_val = keyboard_get_value();
        
        if (key_val != 0)
        {
            debug_printf("You pressed:%c\r\n", key_val);
            sys_send_msg_to_queue(password_queue, MSG_CMD_KEY_INPUT, key_val);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// 核心逻辑层：统筹鉴权比对、模式跳转与安防策略
static void sys_password_process_task(void *pvParameters)
{
    (void)pvParameters;
    system_msg_t recv_msg;
    uint8_t password_mode = PWD_MODE_INPUT;
    uint8_t read_pwd_buffer[10] = {0}; 

    /* 阶段 1: [启动状态探测] 
     * 读取非易失存储器，判断系统处于“出厂未初始化”还是“正常服务”状态
     */
    if (xSemaphoreTake(w25q128_mutex, portMAX_DELAY) == pdTRUE)
    {
        w25q128_read_data(0x000000, read_pwd_buffer, 10);
        xSemaphoreGive(w25q128_mutex);
        debug_printf("Examine password:%s\r\n", read_pwd_buffer);
    }
    
    if (read_pwd_buffer[0] == '0' || read_pwd_buffer[0] == 0xFF)
    {
        sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_SET_PWD, 0);
        password_mode = PWD_MODE_SET;
    }
    else
    {
        sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT, 0);
    }
    
    /* 阶段 2: [事件驱动主循环] 
     * 阻塞接收输入信号，执行鉴权与状态机流转
     */
    while (1)
    {
        if (xQueueReceive(password_queue, &recv_msg, portMAX_DELAY) == pdPASS)
        {
            if (recv_msg.cmd == MSG_CMD_KEY_INPUT)
            {
                // 分支 A：触发修改密码流程 (*)
                if (recv_msg.data == START_KEY)
                {
                    password_mode = PWD_MODE_MODIFY;
                    sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT_OLD, 0);
                    password_input_clear();
                }
                // 分支 B：提交确认请求 (#)
                else if (recv_msg.data == POUND_KEY)
                {
                    // 情境 B-1: 新密码设定落盘
                    if (password_mode == PWD_MODE_SET)
                    {
                        if (xSemaphoreTake(w25q128_mutex, portMAX_DELAY) == pdTRUE)
                        {
                            password_save();
                            xSemaphoreGive(w25q128_mutex);
                            
                            sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_SUCCESS, 0);
                            sys_send_msg_to_queue(beep_queue, MSG_CMD_BEEP_SHORT, 0);
                            vTaskDelay(pdMS_TO_TICKS(1000));
                            
                            password_mode = PWD_MODE_INPUT;
                            sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT, 0);
                        }
                    }
                    // 情境 B-2: 密码鉴权对比 (常态/修改态)
                    else
                    {
                        uint8_t compare_result = FALSE;
                        if (xSemaphoreTake(w25q128_mutex, portMAX_DELAY) == pdTRUE)
                        {
                            compare_result = password_compare();
                            xSemaphoreGive(w25q128_mutex);
                        }
                        
                        // 子情境：常规开门鉴权
                        if (password_mode == PWD_MODE_INPUT)
                        {
                            if (compare_result == TRUE)
                            {
                                xEventGroupSetBits(system_event_group, SYS_EVT_PWD_CORRECT);
                                pwd_error_attempts = 0;
                                
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_RIGHT, 0);
                                sys_send_msg_to_queue(beep_queue, MSG_CMD_BEEP_SHORT, 0);
                                sys_send_msg_to_queue(relay_lock_queue, MSG_CMD_LOCK_OPEN, 0);
                                
                                door_unlocked_status = TRUE;
                                xTimerStart(auto_lock_timer, 0);
                            }
                            else
                            {
                                xEventGroupSetBits(system_event_group, SYS_EVT_PWD_WRONG);
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_WRONG, 0);
                                
                                pwd_error_attempts++;
                                if (pwd_error_attempts >= 3)
                                {
                                    xEventGroupSetBits(system_event_group, SYS_EVT_PWD_ALARM);
                                    sys_send_msg_to_queue(beep_queue, MSG_CMD_BEEP_LONG, 0);
                                    pwd_error_attempts = 0;
                                    vTaskDelay(pdMS_TO_TICKS(3000));
                                }
                                else
                                {
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                }
                                
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT, 0);
                            }
                        }
                        // 子情境：修改旧密码的前置鉴权
                        else if (password_mode == PWD_MODE_MODIFY)
                        {
                            if (compare_result == TRUE)
                            {
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT_NEW, 0);
                                password_mode = PWD_MODE_SET;
                            }
                            else
                            {
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_WRONG, 0);
                                sys_send_msg_to_queue(beep_queue, MSG_CMD_BEEP_SHORT, 0);
                                password_mode = PWD_MODE_INPUT;
                                vTaskDelay(pdMS_TO_TICKS(1000));
                                sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_INPUT, 0);
                            }
                        }
                    }
                    
                    // 清理本地输入缓存状态，迎接下一次交互
                    password_input_clear();
                    pwd_input_index = 0;
                }
                // 分支 C：录入常规数字符号
                else
                {
                    if (pwd_input_index < 10)
                    {
                        password_set_input(recv_msg.data);
                        pwd_input_index++;
                        sys_send_msg_to_queue(display_queue, MSG_CMD_OLED_SHOW_CHAR, recv_msg.data);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// 渲染展示层：统一代理 OLED 绘制权，防止多任务并发导致显存总线冲突
static void sys_oled_display_task(void *pvParameters)
{
    (void)pvParameters;
    system_msg_t recv_msg;
    
    while (1)
    {
        if (xQueueReceive(display_queue, &recv_msg, portMAX_DELAY) == pdPASS)
        {
            switch (recv_msg.cmd)
            {
                case MSG_CMD_OLED_SHOW_SET_PWD:     oled_show_set();                                        break;
                case MSG_CMD_OLED_SHOW_INPUT:       oled_show_input();                                      break;
                case MSG_CMD_OLED_SHOW_RIGHT:       oled_show_right();                                      break;
                case MSG_CMD_OLED_SHOW_WRONG:       oled_show_wrong();                                      break;
                case MSG_CMD_OLED_SHOW_INPUT_NEW:   oled_show_new();                                        break;
                case MSG_CMD_OLED_SHOW_INPUT_OLD:   oled_show_old();                                        break;
                case MSG_CMD_OLED_SHOW_SUCCESS:     oled_show_changed();                                    break;
                case MSG_CMD_OLED_SHOW_CHAR:        oled_show_char(20 + (pwd_input_index - 1) * 10, 4, recv_msg.data, 16); break;
                default:                                                                                    break;
            }
        }
    }
}

// 异常监控层：监听全局安防事件总线，用于后续扩展日志记录或远程报警
static void sys_event_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event_bits;

    while (1)
    {
        event_bits = xEventGroupWaitBits(
            system_event_group,
            SYS_EVT_PWD_CORRECT | SYS_EVT_PWD_WRONG | SYS_EVT_PWD_ALARM,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );
        
        if (event_bits & SYS_EVT_PWD_CORRECT)
        {
            debug_printf("Password verification is correct!\r\n");
        }
        
        if (event_bits & SYS_EVT_PWD_WRONG)
        {
            debug_printf("Password verification failed!\r\n");
        }
        
        if (event_bits & SYS_EVT_PWD_ALARM)
        {
            debug_printf("Warning: Too many input errors!\r\n");
        }
    }
}

// 异步反馈层：剥离包含阻塞延时的蜂鸣器操作，保障核心状态机的实时响应
static void sys_beep_alert_task(void *pvParameters)
{
    (void)pvParameters;
    system_msg_t recv_msg;
    
    while (1)
    {
        if (xQueueReceive(beep_queue, &recv_msg, portMAX_DELAY) == pdPASS)
        {
            switch (recv_msg.cmd)
            {
                case MSG_CMD_BEEP_SHORT:
                    beep_on();
                    vTaskDelay(pdMS_TO_TICKS(300));
                    beep_off();
                    debug_printf("Beep short sing!");
                    break;
                case MSG_CMD_BEEP_LONG:
                    beep_on();
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    beep_off();
                    debug_printf("Beep long sing!");
                    break;
                default:
                    break;
            }
        }
    }
}

// 物理执行层：隔离慢速或涉及高功率的机械继电器操作
static void sys_relay_lock_task(void *pvParameters)
{
    (void)pvParameters;
    system_msg_t recv_msg;
    
    while (1)
    {
        if (xQueueReceive(relay_lock_queue, &recv_msg, portMAX_DELAY) == pdPASS)
        {
            switch (recv_msg.cmd)
            {
                case MSG_CMD_LOCK_OPEN:
                    relay_lock_on();
                    debug_printf("Open lock successfully!");
                    break;
                case MSG_CMD_LOCK_CLOSE:
                    relay_lock_off();
                    debug_printf("Close lock successfully!"); 
                    break;
                default:
                    break;
            }
        }
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 资源孵化与生命周期起点                                                */
/* ---------------------------------------------------------------------------------- */

void app_sys_orchestrator_init(void)
{
    password_queue      = xQueueCreate(10, sizeof(system_msg_t));
    display_queue       = xQueueCreate(10, sizeof(system_msg_t));
    beep_queue          = xQueueCreate(10, sizeof(system_msg_t));
    relay_lock_queue    = xQueueCreate(10, sizeof(system_msg_t));
    
    w25q128_mutex       = xSemaphoreCreateMutex();
    system_event_group  = xEventGroupCreate();
    
    auto_lock_timer     = xTimerCreate(
                            "auto_lock",
                            pdMS_TO_TICKS(3000),
                            pdFALSE,
                            NULL,
                            sys_auto_lock_timer_cb
                          );
    
    xTaskCreate(sys_keyboard_scan_task,     "kb_scan_tsk",      128, NULL, 1, &keyboard_task_handle);
    xTaskCreate(sys_password_process_task,  "pwd_proc_tsk",     256, NULL, 1, &password_task_handle);
    xTaskCreate(sys_oled_display_task,      "oled_disp_tsk",    128, NULL, 1, &display_task_handle);
    xTaskCreate(sys_event_monitor_task,     "evt_mon_tsk",      128, NULL, 1, &monitor_task_handle);
    xTaskCreate(sys_beep_alert_task,        "beep_alrt_tsk",    128, NULL, 1, &beep_task_handle);
    xTaskCreate(sys_relay_lock_task,        "relay_lk_tsk",     128, NULL, 1, &relay_lock_task_handle);
}

