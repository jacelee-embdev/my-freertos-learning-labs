#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "timers.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t  task1_handle  = NULL;                              // 按键扫描与事件分发任务句柄
static TimerHandle_t timer1_handle = NULL;                              // 软件定时器 1 句柄
static TimerHandle_t timer2_handle = NULL;                              // 软件定时器 2 句柄

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层] 定时器回调业务逻辑                                                    */
/* ---------------------------------------------------------------------------------- */

// 统一软件定时器回调函数 (通过 ID 区分不同定时器逻辑)
static void timer_callback(TimerHandle_t xTimer)
{
    static uint32_t cnt1 = 0;
    static uint32_t cnt2 = 0;
    
    // 获取当前定时器的 ID 标识
    uint32_t timer_id = (uint32_t)pvTimerGetTimerID(xTimer);
    
    if (timer_id == 1)
    {
        debug_printf("[timer1] 运行次数:%d\r\n", ++cnt1);
    }
    else if (timer_id == 2)
    {
        debug_printf("[timer2] 运行次数:%d\r\n", ++cnt2);
    }
}

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 按键检测与事件分发任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_num = 0;
    
    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 触发有效按键时切换对应定时器的运行状态 */
        if (key_num == 1)
        {
            if (xTimerIsTimerActive(timer1_handle) == pdFALSE)
            {
                debug_printf("按键1按下, 启动定时器1\r\n");
                xTimerStart(timer1_handle, portMAX_DELAY);
            }
            else
            {
                debug_printf("按键1按下, 停止定时器1\r\n");
                xTimerStop(timer1_handle, portMAX_DELAY);
            }
        }
        else if (key_num == 2)
        {
            if (xTimerIsTimerActive(timer2_handle) == pdFALSE)
            {
                debug_printf("按键2按下, 启动定时器2\r\n");
                xTimerStart(timer2_handle, portMAX_DELAY);
            }
            else
            {
                debug_printf("按键2按下, 停止定时器2\r\n");
                xTimerStop(timer2_handle, portMAX_DELAY);
            }
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建单次定时器 (周期 500ms, ID 为 1) */
    timer1_handle = xTimerCreate("timer1",
                                 5000,
                                 pdFALSE,
                                 (void *)1,
                                 timer_callback);
     
    /* 阶段 2: 创建周期定时器 (周期 1000ms, ID 为 2) */
    timer2_handle = xTimerCreate("timer2",
                                 1000,
                                 pdTRUE,
                                 (void *)2,
                                 timer_callback);
    
    /* 阶段 3: 创建按键扫描业务任务 */
    xTaskCreate(task1,
                "task1",
                128,
                NULL,
                1,
                &task1_handle);
}
