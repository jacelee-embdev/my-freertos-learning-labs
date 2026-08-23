#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "timers.h"
#include <stdio.h>

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄]                                                                   */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t  task1_handle  = NULL;                              // 按键扫描与事件分发任务句柄
static TaskHandle_t  task2_handle  = NULL;                              // 定时器状态监控任务句柄
static TimerHandle_t timer1_handle = NULL;                              // 软件定时器 1 句柄 (单次)
static TimerHandle_t timer2_handle = NULL;                              // 软件定时器 2 句柄 (周期)

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层]                                                                       */
/* ---------------------------------------------------------------------------------- */

// 统一软件定时器回调函数 (通过 ID 区分不同定时器逻辑)
static void timer_callback(TimerHandle_t xTimer)
{
    static uint32_t cnt1 = 0;
    static uint32_t cnt2 = 0;
    
    /* 阶段 1: 获取当前触发定时器的 ID 标识 */
    uint32_t timer_id = (uint32_t)pvTimerGetTimerID(xTimer);
    
    /* 阶段 2: 根据 ID 执行对应业务逻辑 */
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
/* [任务主体区]                                                                       */
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

// 定时器状态监控与打印任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    while(1)
    {
        /* 阶段 1: 打印定时器运行信息 */
        debug_printf("距离[timer2]下次到期时间为:%d\r\n", xTimerGetExpiryTime(timer2_handle) - xTaskGetTickCount());
        debug_printf("[timer1]定时器周期为:%d,定时器ID为:%d\r\n", xTimerGetPeriod(timer1_handle), (int)pvTimerGetTimerID(timer1_handle));
        
        /* 阶段 2: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区]                                                                       */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建单次软件定时器 (周期 5000ms, ID 为 1) */
    timer1_handle = xTimerCreate("timer1",
                                 pdMS_TO_TICKS(5000),
                                 pdFALSE,
                                 (void *)1,
                                 timer_callback);
     
    /* 阶段 2: 创建周期软件定时器 (周期 1000ms, ID 为 2) */
    timer2_handle = xTimerCreate("timer2",
                                 pdMS_TO_TICKS(1000),
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

    /* 阶段 4: 创建状态监控业务任务 */
    xTaskCreate(task2,
                "task2",
                128,
                NULL,
                1,
                &task2_handle);
}
