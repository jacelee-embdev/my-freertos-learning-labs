// app_sys_orchestrator.c
#include "app_sys_orchestrator.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "lcd1602.h"
#include "beep.h"
#include "relay_gate.h"
#include "string.h"
#include "exti.h"
#include <stdio.h>

/* ---------------------------------------------------------------------------------- */
/* [配置参数区] 模块内部使用的宏定义与枚举                                            */
/* ---------------------------------------------------------------------------------- */

#define NOTIFY_WAIT_TIMEOUT 0x01 // 任务通知掩码：等待状态超时重置
#define NOTIFY_PERSON_PASS  0x02 // 任务通知掩码：单次人员通行触发

#define MAX_PASSENGERS      5    // 业务阈值：系统锁定前的最大允许通行人次

// 系统核心状态机定义
enum
{
    SYS_STATE_ALLOW_PASS,        // 开放态：允许人员通行，闸门常规响应
    SYS_STATE_LOCKED_WAIT        // 锁定态：名额耗尽，拒绝通行并启动超时恢复机制
};

// 显示屏消息帧结构体
typedef struct
{
    uint8_t row;                 // 目标显示行 (1 或 2)
    uint8_t col;                 // 目标显示列首地址
    char    message[16];         // 格式化后的字符串数据
} DisplayMessage_t;

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄与全局变量                              */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      sensor_task_handle        = NULL; // 传感器监控任务句柄
static TaskHandle_t      gate_actuator_task_handle = NULL; // 闸门与声光执行器任务句柄
static TaskHandle_t      display_task_handle       = NULL; // LCD 刷新任务句柄

static QueueHandle_t     display_queue;                    // 跨任务 UI 渲染数据流队列
static QueueHandle_t     state_queue;                      // 跨任务状态机同步队列

static TimerHandle_t     wait_timer;                       // 锁定态恢复倒计时软件定时器

static SemaphoreHandle_t lcd_mutex;                        // LCD 底层总线防冲突互斥锁
static SemaphoreHandle_t passage_semaphore;                // 通行名额资源池 (计数型信号量)

static uint8_t           system_state              = SYS_STATE_ALLOW_PASS; // 记录当前系统所处的主状态

/* ---------------------------------------------------------------------------------- */
/* [系统回调层] 硬件中断或软件定时器回调                                              */
/* ---------------------------------------------------------------------------------- */

// 锁定状态超时回调 (由 FreeRTOS Daemon 任务调度执行，禁止阻塞)
void wait_timer_callback(TimerHandle_t xTimer)
{
    // 向传感器主任务发送“解除锁定”的任务通知
    if (sensor_task_handle != NULL)
    {
        xTaskNotify(sensor_task_handle, NOTIFY_WAIT_TIMEOUT, eSetBits);
    }
}

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 传感器与状态机管控任务 (系统的“大脑”)
static void sensor_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t          current_state;
    uint8_t          available_passages = MAX_PASSENGERS; // 本地化维护剩余名额，减少全局竞态
    uint32_t         notification_val;
    DisplayMessage_t display_message;

    /* 阶段 1: 系统上电初始化与状态首发
     * 确立默认状态为允许通行，并将初始 UI 界面推入显示队列，驱动 LCD 刷新
     */
    system_state = SYS_STATE_ALLOW_PASS;
    xQueueSend(state_queue, &system_state, 0);
    
    sprintf(display_message.message, "PASS...00/05");
    display_message.row = 1;
    display_message.col = 1;
    xQueueSend(display_queue, &display_message, 0);

    while(1)
    {
        // 抓取当前系统快照状态
        current_state = system_state;
        
        /* 阶段 2: 硬件触发检测与名额消耗
         * 仅在允许通行状态下响应红外中断标志。通过 Take 信号量安全地扣减名额，
         * 彻底避免多重并发触发导致的计数器异常。
         */
        if (current_state == SYS_STATE_ALLOW_PASS && get_infrared_sensor_flag() == TRUE)
        {
            set_infrared_sensor_flag(FALSE); // 及时清零底层硬件标志
            
            // 尝试从资源池中获取一个通行权 (非阻塞)
            if (xSemaphoreTake(passage_semaphore, 0) == pdTRUE)
            {
                available_passages--;
                
                // 拼接并下发更新后的实时人数信息
                sprintf(display_message.message, "PASS...%02d/05", MAX_PASSENGERS - available_passages);
                display_message.row = 1;
                display_message.col = 1;
                xQueueSend(display_queue, &display_message, 0);
                
                /* 阶段 3: 阈值阻断与状态切换
                 * 当名额耗尽时，主动拉高系统防御级别，切入锁定态，广播给其他执行器任务
                 */
                if (available_passages == 0)
                {
                    system_state = SYS_STATE_LOCKED_WAIT;
                    xQueueSend(state_queue, &system_state, 0); // 同步状态给声光/继电器任务
                    
                    sprintf(display_message.message, "WAIT...00/05");
                    display_message.row = 1;
                    display_message.col = 1;
                    xQueueSend(display_queue, &display_message, 0);
                }
            }
        }
        
        /* 阶段 4: 阻塞等待异步唤醒 (软定时器超时)
         * 使用轻量级的任务通知代替全局标志位轮询，接收来自 wait_timer 的重置指令
         */
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &notification_val, 0) == pdTRUE)
        {
            // 校验通知掩码是否为超时重置信号
            if ((notification_val & NOTIFY_WAIT_TIMEOUT) != 0)
            {
                if (current_state == SYS_STATE_LOCKED_WAIT)
                {
                    available_passages = MAX_PASSENGERS;
                    
                    // 容错处理：抽干可能因异常抖动残留的信号量，确保水位线归零
                    while (xSemaphoreTake(passage_semaphore, 0) == pdTRUE)
                    {
                    }
                    
                    // 重新补满通行权资源池
                    for (int i = 0; i < MAX_PASSENGERS; i++)
                    {
                        xSemaphoreGive(passage_semaphore);
                    }
                    
                    set_infrared_sensor_flag(FALSE); // 防止锁定期间积累的错误触发
                    
                    // 广播恢复至通行状态
                    system_state = SYS_STATE_ALLOW_PASS;
                    xQueueSend(state_queue, &system_state, 0);
                    
                    sprintf(display_message.message, "PASS...00/05");
                    display_message.row = 1;
                    display_message.col = 1;
                    xQueueSend(display_queue, &display_message, 0);
                }
            }
        }
        
        // 释放 CPU 使用权，设定 20ms 的业务轮询基准节拍
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// 闸门执行器任务 (系统的“手脚”：处理外设动作)
static void gate_actuator_task(void *pvParameters)
{
    (void)pvParameters;
    uint16_t blink_tick_counter = 0; // 配合延时节拍器实现软定时的计步器
    uint8_t  current_state      = SYS_STATE_ALLOW_PASS;

    while(1)
    {
        /* 阶段 1: 监听并同步系统全局状态
         * 接收大脑(sensor_monitor_task)发来的状态机变更指令，并据此启停重置定时器
         */
        if (xQueueReceive(state_queue, &current_state, 0) == pdPASS)
        {
            if (current_state == SYS_STATE_LOCKED_WAIT)
            {
                xTimerStart(wait_timer, 0); // 触发 3s 的锁定倒计时
            }
            else if (current_state == SYS_STATE_ALLOW_PASS)
            {
                xTimerStop(wait_timer, 0);
            }
            
            // 状态切换时立即强制执行一次硬件动作，提高响应顺滑度
            blink_tick_counter = 100;
        }
        
        /* 阶段 2: 根据所处状态执行解耦的硬件动作
         * 利用软计数器分频，实现不同的 LED 闪烁频率和声光特征
         */
        if (current_state == SYS_STATE_ALLOW_PASS)
        {
            // 10ms * 100 = 1000ms 动作周期
            if (blink_tick_counter >= 100)
            {                                                                                                                                                   
                led1_toggle();    // 待机指示灯慢闪
                led2_off();
                beep_off();
                relay_gate_on();  // 保持闸机常开/允许推入
                blink_tick_counter = 0;
            }
            else
            {
                blink_tick_counter++;
            }
        }
        else if (current_state == SYS_STATE_LOCKED_WAIT)
        {
            // 10ms * 20 = 200ms 动作周期 (急促警报感)
            if (blink_tick_counter >= 20)
            {
                led1_off();
                led2_toggle();    // 警报指示灯快闪
                beep_on();        // 蜂鸣器长鸣
                relay_gate_off(); // 强制断开继电器，锁死闸机
                blink_tick_counter = 0;
            }
            else
            {
                blink_tick_counter++;
            }
        }
        
        // 设定外设响应的底层刷新率为 10ms
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// 独立 UI 渲染任务 (系统的“脸面”：集中处理所有显示冲突)
static void display_task(void *pvParameters)
{
    (void)pvParameters;
    DisplayMessage_t display_message;

    while(1)
    {
        /* 核心机制: 数据流驱动与外设锁
         * 死等显示队列，彻底杜绝无数据时的 CPU 浪费。
         * 获取到数据后，必须申请 LCD 互斥锁，防止与其他可能操作屏幕的任务产生 IIC/SPI 时序碰撞。
         */
        if (xQueueReceive(display_queue, &display_message, portMAX_DELAY) == pdPASS)
        {
            if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE)
            {
                lcd1602_show_char_string(display_message.row, display_message.col, display_message.message);
                xSemaphoreGive(lcd_mutex);
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
    // 创建通讯队列 (深度为 3 留有缓冲余量)
    state_queue       = xQueueCreate(3, sizeof(uint8_t));
    display_queue     = xQueueCreate(3, sizeof(DisplayMessage_t));
    
    // 创建 LCD 互斥量，保护非重入的底层驱动协议
    lcd_mutex         = xSemaphoreCreateMutex();
    
    // 初始化通行资源池，最大容量 5，初始满载
    passage_semaphore = xSemaphoreCreateCounting(MAX_PASSENGERS, MAX_PASSENGERS); 
    
    // 创建 3000ms 的单次软件定时器，用于锁定态的自动恢复
    wait_timer        = xTimerCreate("wait_timer",
                                     pdMS_TO_TICKS(3000),
                                     pdFALSE,
                                     (void *)0,
                                     &wait_timer_callback);
    
    // 实例化系统任务群，默认采用同级优先级，依靠时间片与阻塞让出 CPU
    xTaskCreate(sensor_monitor_task,
                "sensor_task",
                128,
                NULL,
                1,
                &sensor_task_handle);
    
    xTaskCreate(gate_actuator_task,
                "actuator_task",
                128,
                NULL,
                1,
                &gate_actuator_task_handle);
    
    xTaskCreate(display_task,
                "display_task",
                128,
                NULL,
                1,
                &display_task_handle);
}
