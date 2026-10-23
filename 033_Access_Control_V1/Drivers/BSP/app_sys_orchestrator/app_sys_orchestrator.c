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

//定义消息结构
typedef struct{
    uint8_t cmd;
    uint8_t data;
}System_Msg_t;

//系统状态
enum
{
    INPUT_MODE,
    MODIF_MODE,
    SET_MODE
};

//按键命令
#define CMD_KEY_INPUT   0x01

//OLED显示控制命令
#define CMD_OLED_SHOW_SET_PWD                   0x01    //显示"请设定密码"
#define CMD_OLED_SHOW_INPUT                     0x02    //显示"请输入密码"
#define CMD_OLED_SHOW_RIGHT                     0x03    //显示"密码正确"
#define CMD_OLED_SHOW_WRONG                     0x04    //显示"密码错误"
#define CMD_OLED_SHOW_INPUT_NEW_PWD             0x05    //显示"请输入新密码"
#define CMD_OLED_SHOW_INPUT_OLD_PWD             0x06    //显示"请输入旧密码"
#define CMD_OLED_SHOW_PWD_CHANGED_SUCCESS       0x07    //显示"密码已修改"
#define CMD_OLED_SHOW_CHAR                      0x08    //显示字符串


//系统事件标志位
#define EVENT_PWD_CORRECT   (1 << 0)    //密码正确事件
#define EVENT_PWD_WRONG     (1 << 1)    //密码错误事件
#define EVENT_PWD_ALARM     (1 << 2)    //报警事件

//蜂鸣器控制命令
#define CMD_BEEP_SHORT      0x01    //短鸣
#define CMD_BEEP_LONG       0x02    //长鸣

//门锁控制命令
#define CMD_LOCK_OPEN       0x01
#define CMD_LOCK_CLOSE      0x02

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄与全局变量                              */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      keyboard_task_handle       = NULL;
static TaskHandle_t      password_task_handle       = NULL;
static TaskHandle_t      display_task_handle        = NULL;
static TaskHandle_t      monitor_task_handle        = NULL;
static TaskHandle_t      beep_task_handle           = NULL;
static TaskHandle_t      relay_lock_task_handle     = NULL;

static QueueHandle_t     password_queue;
static QueueHandle_t     display_queue;
static QueueHandle_t     beep_queue;
static QueueHandle_t     relay_lock_queue;

static TimerHandle_t     auto_lock_timer;

static SemaphoreHandle_t w25q128_mutex;
static EventGroupHandle_t system_event_group;

static uint8_t pwd_index    = 0; //密码缓冲区索引
static uint8_t attempt_cnt  = 0; //尝试次数计数器
static uint8_t door_status  = TRUE;


static void send_msg_by_queue(QueueHandle_t queue,uint8_t cmd,uint8_t data)
{
    System_Msg_t recv_msg;
    
    recv_msg.cmd = cmd;
    recv_msg.data = data;
    
    xQueueSend(queue,&recv_msg,portMAX_DELAY);
}
/* ---------------------------------------------------------------------------------- */
/* [系统回调层] 硬件中断或软件定时器回调                                              */
/* ---------------------------------------------------------------------------------- */

// 锁定状态超时回调 (由 FreeRTOS Daemon 任务调度执行，禁止阻塞)
void auto_timer_callback(TimerHandle_t xTimer)
{
    if(door_status == TRUE)
    {
        //发送关门通知
        send_msg_by_queue(relay_lock_queue,CMD_LOCK_CLOSE,0);
        
        //更新显示
        send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT,0);
        
        //更新门的状态
        door_status = FALSE;
    }
    
}

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

static void keyboard_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_val;

    while(1)
    {
        //获取按键值
        key_val = keyboard_get_value();
        
        //处理按键被按下的事件
        if(key_val != 0)
        {
            debug_printf("You pressed:%c\r\n",key_val);
            send_msg_by_queue(password_queue,CMD_KEY_INPUT,key_val);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void password_task(void *pvParameters)
{
    (void)pvParameters;
    System_Msg_t recv_msg;
    uint8_t password_mode = INPUT_MODE;
    uint8_t pwd_temp[10] = {0}; 

    //检查密码是否已经设置
    if(xSemaphoreTake(w25q128_mutex,portMAX_DELAY) == pdTRUE)
    {
        w25q128_read_data(0x000000,pwd_temp,10);
        xSemaphoreGive(w25q128_mutex);
        debug_printf("Examine password:%s\r\n",pwd_temp);
    }
    
    if(pwd_temp[0] == '0' || pwd_temp[0] == 0xFF)
    {
        //未设置密码
        send_msg_by_queue(display_queue,CMD_OLED_SHOW_SET_PWD,0);
        password_mode = SET_MODE;
    }
    else
    {
        //密码已经设置
        send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT,0);
    }
    
    while(1)
    {
        //等待消息
        if(xQueueReceive(password_queue,&recv_msg,portMAX_DELAY) == pdPASS)
        {
            //判断是否是按键命令
            if(recv_msg.cmd == CMD_KEY_INPUT)
            {
                //修改键 *
                if(recv_msg.data == START_KEY)
                {
                    password_mode = MODIF_MODE;
                    send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT_OLD_PWD,0);
                    password_input_clear();
                }
                //确认键 #
                else if(recv_msg.data == POUND_KEY)
                {
                    //处于设置新密码模式
                    if(password_mode == SET_MODE)
                    {
                        if(xSemaphoreTake(w25q128_mutex,portMAX_DELAY) == pdTRUE)
                        {
                            password_save();
                            xSemaphoreGive(w25q128_mutex);
                            
                            send_msg_by_queue(display_queue,CMD_OLED_SHOW_PWD_CHANGED_SUCCESS,0);
                            send_msg_by_queue(beep_queue,CMD_BEEP_SHORT,0);
                            vTaskDelay(pdMS_TO_TICKS(1000));
                            
                            password_mode = INPUT_MODE;
                            send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT,0);
                        }
                    }
                    //处于修改密码模式
                    else
                    {
                        uint8_t compare_result = FALSE;
                        if(xSemaphoreTake(w25q128_mutex,portMAX_DELAY) == pdTRUE)
                        {
                            compare_result = password_compare();
                            xSemaphoreGive(w25q128_mutex);
                        }
                        
                        if(password_mode == INPUT_MODE)
                        {
                            if(compare_result == TRUE)
                            {
                                //设置密码输入正确事件
                                xEventGroupSetBits(system_event_group,EVENT_PWD_CORRECT);
                                
                                //重置密码错误计数器
                                attempt_cnt =0;
                                
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_RIGHT,0);
                                send_msg_by_queue(beep_queue,CMD_BEEP_SHORT,0);
                                
                                //开锁
                                send_msg_by_queue(relay_lock_queue,CMD_LOCK_OPEN,0);
                                door_status = TRUE;
                                
                                //启动定时器
                                xTimerStart(auto_lock_timer,0);
                            }
                            else
                            {
                                //设置密码输入错误事件
                                xEventGroupSetBits(system_event_group,EVENT_PWD_WRONG);
                                
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_WRONG,0);
                                
                                //更新密码错误计数器
                                attempt_cnt++;
                                if(attempt_cnt >= 3 )
                                {
                                    //设置警告事件
                                    xEventGroupSetBits(system_event_group,EVENT_PWD_ALARM);
                                    send_msg_by_queue(beep_queue,CMD_BEEP_LONG,0);
                                    attempt_cnt = 0;
                                    vTaskDelay(pdMS_TO_TICKS(3000));
                                }
                                else
                                {
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                }
                                
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT,0);
                            }
                        }
                        else if(password_mode == MODIF_MODE)
                        {
                            if(compare_result == TRUE)
                            {
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT_NEW_PWD,0);
                                password_mode = SET_MODE;
                            }
                            else
                            {
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_WRONG,0);
                                send_msg_by_queue(beep_queue,CMD_BEEP_SHORT,0);
                                password_mode = INPUT_MODE;
                                vTaskDelay(pdMS_TO_TICKS(1000));
                                send_msg_by_queue(display_queue,CMD_OLED_SHOW_INPUT,0) ;
                            }
                        }
                    }
                    //处于开门模式
                    
                    password_input_clear();
                    pwd_index = 0;
                }
                //其它键 
                else
                {
                    if(pwd_index < 10)
                    {
                        password_set_input(recv_msg.data);
                        pwd_index++;
                        send_msg_by_queue(display_queue,CMD_OLED_SHOW_CHAR,recv_msg.data);
                    }
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void display_task(void *pvParameters)
{
    (void)pvParameters;

    System_Msg_t recv_msg;
    
    while(1)
    {
        //等待门锁控制消息
        if(xQueueReceive(display_queue,&recv_msg,portMAX_DELAY) == pdPASS)
        {
            switch(recv_msg.cmd)
            {
                case CMD_OLED_SHOW_SET_PWD:
                    oled_show_set();
                    break;
                case CMD_OLED_SHOW_INPUT:
                    oled_show_input();
                    break;
                case CMD_OLED_SHOW_RIGHT:
                    oled_show_right();
                    break;
                case CMD_OLED_SHOW_WRONG:
                    oled_show_wrong();
                    break;
                case CMD_OLED_SHOW_INPUT_NEW_PWD:
                    oled_show_new();
                    break;
                case CMD_OLED_SHOW_INPUT_OLD_PWD:
                    oled_show_old();
                    break;
                case CMD_OLED_SHOW_PWD_CHANGED_SUCCESS:
                    oled_show_changed();
                    break;
                case CMD_OLED_SHOW_CHAR:
                    oled_show_char(20+(pwd_index-1)*10,4,recv_msg.data,16);
                    break;
                default:
                    break;
            }
        }
    }
}

static void monitor_task(void *pvParameters)
{
    (void)pvParameters;
    EventBits_t event_bits;

    while(1)
    {
        event_bits = xEventGroupWaitBits(
        system_event_group,
        EVENT_PWD_CORRECT | EVENT_PWD_WRONG | EVENT_PWD_ALARM,
        pdTRUE,
        pdFALSE,
        portMAX_DELAY);
        
        //处理密码正确事件
        if(event_bits & EVENT_PWD_CORRECT)
        {
            debug_printf("Password verification is correct!\r\n");
        }
        
        //处理密码错误事件
        if(event_bits & EVENT_PWD_WRONG)
        {
            debug_printf("Password verification failed!\r\n");
        }
        
        //处理报警事件
        if(event_bits & EVENT_PWD_ALARM)
        {
            debug_printf("Warning: Too many input errors!\r\n");
        }
    }
}

static void beep_task(void *pvParameters)
{
    (void)pvParameters;

    System_Msg_t recv_msg;
    
    while(1)
    {
        //等待门锁控制消息
        if(xQueueReceive(beep_queue,&recv_msg,portMAX_DELAY) == pdPASS)
        {
            switch(recv_msg.cmd)
            {
                case CMD_BEEP_SHORT:
                    beep_on();
                    vTaskDelay(pdMS_TO_TICKS(300));
                    beep_off();
                    debug_printf("Beep short sing!");
                    break;
                case CMD_BEEP_LONG:
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

static void relay_lock_task(void *pvParameters)
{
    (void)pvParameters;

    System_Msg_t recv_msg;
    
    while(1)
    {
        //等待门锁控制消息
        if(xQueueReceive(relay_lock_queue,&recv_msg,portMAX_DELAY) == pdPASS)
        {
            switch(recv_msg.cmd)
            {
                case CMD_LOCK_OPEN:
                    relay_lock_on();
                    debug_printf("Open lock successfully!");
                    break;
                case CMD_LOCK_CLOSE:
                    relay_lock_off();
                    debug_printf("Open lock unsuccessfully!");
                    break;
                default:
                    break;
            }
        }
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统核心编排器 (统筹创建各个子任务与 OS 资源)
void app_sys_orchestrator_init(void)
{
    //创建队列
    password_queue = xQueueCreate(10,sizeof(System_Msg_t));
    display_queue = xQueueCreate(10,sizeof(System_Msg_t));
    beep_queue = xQueueCreate(10,sizeof(System_Msg_t));
    relay_lock_queue = xQueueCreate(10,sizeof(System_Msg_t));
//    state_queue = xQueueCreate(10,sizeof(System_Msg_t));
    
    //创建互斥量
    w25q128_mutex = xSemaphoreCreateMutex();
    
    //创建时间标志组
    system_event_group = xEventGroupCreate();
    
    //创建定时器
    auto_lock_timer = xTimerCreate(
                                    "auto_lock_timer",
                                    pdMS_TO_TICKS(3000),
                                    pdFALSE,
                                    NULL,
                                    auto_timer_callback);
    
    //创建任务
    xTaskCreate(keyboard_task,
                "keyboard_task",
                128,
                NULL,
                1,
                &keyboard_task_handle);
    
    xTaskCreate(password_task,
                "password_task",
                256,
                NULL,
                1,
                &password_task_handle);
    
    xTaskCreate(display_task,
                "display_task",
                128,
                NULL,
                1,
                &display_task_handle);
    
    xTaskCreate(monitor_task,
                "monitor_task",
                128,
                NULL,
                1,
                &monitor_task_handle);
                
    xTaskCreate(beep_task,
                "beep_task",
                128,
                NULL,
                1,
                &beep_task_handle);
                
    xTaskCreate(relay_lock_task,
                "relay_lock_task",
                128,
                NULL,
                1,
                &relay_lock_task_handle);
}
