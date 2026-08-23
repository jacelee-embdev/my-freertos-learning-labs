#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "freertos_test.h"

TaskHandle_t            task1_handle;   /* 任务句柄 */
TaskHandle_t            task2_handle;   /* 任务句柄 */

void task1(void *pvParameters)
{
    while(1)
    {
        led1_on();
        vTaskDelay(1000);
        led1_off();
        vTaskDelay(1000);
    }
}

void task2(void *pvParameters)
{
    while(1)
    {
        led2_on();
        vTaskDelay(500);
        led2_off();
        vTaskDelay(500);
    }
}

void freertos_test(void)
{
    xTaskCreate(task1,                  /* 任务函数 */
                "task1",                /* 任务名称 */
                128,                    /* 任务堆栈大小 */
                NULL,                   /* 传入给任务函数的参数 */
                2,                      /* 任务优先级 */
                &task1_handle);         /* 任务句柄 */

    xTaskCreate(task2,                  /* 任务函数 */
                "task2",                /* 任务名称 */
                128,                    /* 任务堆栈大小 */
                NULL,                   /* 传入给任务函数的参数 */
                3,                      /* 任务优先级 */
                &task2_handle);         /* 任务句柄 */
}


