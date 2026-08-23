#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */
// 句柄使用 static 修饰，实现本文件内的物理隔离，防止外部非法篡改
TaskHandle_t led1_task_handle = NULL; //由于需要在外部 extern调用它，所以它的静态修饰已经被解除了
static TaskHandle_t led2_task_handle = NULL;
static TaskHandle_t key_monitor_task_handle = NULL;

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

// 任务主体：LED1 状态慢速闪烁指示 | 优先级：1 | 栈深度：128字
static void led1_task(void *pvParameters)
{
    (void)pvParameters; // 规避编译器"参数未使用"警告

    while(1)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000)); // 消除魔术数字，确保不同系统 Tick 下时序一致
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 任务主体：LED2 状态快速闪烁指示 | 优先级：1 | 栈深度：128字
static void led2_task(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        led2_on();
        vTaskDelay(pdMS_TO_TICKS(500));
        led2_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// 任务主体：按键轮询监控与任务动态管理 | 优先级：1 | 栈深度：128字
static void key_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_num = 0;

    while(1)
    {
        key_num = key_get_active();
        
        if(key_num == 1)
        {
            if(led1_task_handle != NULL)
            {
                printf("挂起 led1_task\r\n");
                vTaskSuspend(led1_task_handle);
            }
        }
        
        // 适当阻塞让出 CPU 资源 (10ms 扫描周期对按键轮询更为合理)
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    // 严格遵循像素级垂直对齐排版
    xTaskCreate(
        led1_task,                  // 1.任务入口函数
        "led1_task",                // 2.任务名称 (字符串保持与函数名一致，方便调试)
        128,                        // 3.堆栈深度(单位:字)
        NULL,                       // 4.任务入口参数
        1,                          // 5.任务优先级
        &led1_task_handle           // 6.任务句柄
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
