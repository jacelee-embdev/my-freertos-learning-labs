#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t led1_task_handle        = NULL;     // LED1 任务句柄
static TaskHandle_t led2_task_handle        = NULL;     // LED2 任务句柄
static TaskHandle_t key_monitor_task_handle = NULL;     // 按键监控任务句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */

// LED1 状态慢速闪烁指示 (优先级: 1, 栈深度: 128字)
static void led1_task(void *pvParameters)
{
    (void)pvParameters;
    debug_printf("led1_task 运行中...\r\n");

    while(1)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// LED2 状态快速闪烁指示 (优先级: 1, 栈深度: 128字)
static void led2_task(void *pvParameters)
{
    (void)pvParameters;
    debug_printf("led2_task 运行中...\r\n");

    while(1)
    {
        led2_on();
        vTaskDelay(pdMS_TO_TICKS(500));
        
        led2_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// 按键轮询监控与任务动态管理 (优先级: 1, 栈深度: 128字)
static void key_monitor_task(void *pvParameters)
{
    uint8_t key_num = 0;
    (void)pvParameters;
    debug_printf("key_monitor_task 运行中...\r\n");

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态动态管理 LED1 任务 */
        if(key_num == 1)
        {
            if(led1_task_handle != NULL)
            {
                printf("删除 led1_task\r\n");
                vTaskDelete(led1_task_handle);
                led1_task_handle = NULL;            // 任务删除后句柄显式置空
            }
            else
            {
                printf("led1_task 已经不存在了，无需重复删除\r\n");
            }
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (10ms 扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    xTaskCreate(
        led1_task,
        "led1_task",
        128,
        NULL,
        1,
        &led1_task_handle
    );
    
    xTaskCreate(
        led2_task,
        "led2_task",
        128,
        NULL,
        1,
        &led2_task_handle
    );
    
    xTaskCreate(
        key_monitor_task,
        "key_monitor",
        128,
        NULL,
        1,
        &key_monitor_task_handle
    );
}