#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */
// 句柄使用 static 修饰，实现本文件内的物理隔离，防止外部非法篡改
static TaskHandle_t led1_task_handle = NULL;
static TaskHandle_t led2_task_handle = NULL;

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

// 任务主体：LED1 状态慢速闪烁指示 | 优先级：1 | 栈深度：128字
static void led1_task(void *pvParameters)
{
    (void)pvParameters; // 规避编译器"参数未使用"警告

    while(1)
    {
        led1_toggle();
        
        delay_ms(20);          /* 模拟业务逻辑耗时 */
        
        vTaskDelay(pdMS_TO_TICKS(500)); // 消除魔术数字，确保不同系统 Tick 下时序一致
    }
}

// 任务主体：LED2 状态快速闪烁指示 | 优先级：1 | 栈深度：128字
static void led2_task(void *pvParameters)
{
    /* 定义记忆时间基准变量 */
    TickType_t xLastWakeTime;
    /* 定义延时执行状态变量 */
    BaseType_t xWasDelayed;
    
    /* 获取当前系统节拍初值 */
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        led2_toggle();
        
        /* 模拟业务逻辑耗时 */
        delay_ms(20); 

        /* 调用绝对延时并获取执行状态 */
        xWasDelayed = xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));

        /* 校验任务是否出现过载 (即延时是否因耗时过长而失效) */
        if (xWasDelayed == pdFALSE)
        {
            /* 在此处添加任务频率失效的处理逻辑 */
            debug_printf("任务过载!!!\r\n");
        }
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
}
