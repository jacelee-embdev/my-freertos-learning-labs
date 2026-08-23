/**
 * @file    freertos_test.c
 * @brief   FreeRTOS 综合测试模块 (原始命名保留版)
 */

#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* 硬件底层支持 */
#include "led.h"
#include "key.h"
#include "uart1.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部 OS 资源                           */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;
static TaskHandle_t task3_handle = NULL;

static QueueHandle_t queue       = NULL; // 增加 static 修饰，防止外部非法篡改

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief 任务主体：LED 闪烁指示 | 优先级：1 | 栈深度：128字
 */
static void task1(void *pvParameters)
{
    (void)pvParameters; // 消除未引用警告
    debug_printf("task1 运行中...\r\n");
    
    for(;;)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief 任务主体：队列接收 | 优先级：1 | 栈深度：128字
 */
static void task2(void *pvParameters)
{
    (void)pvParameters;
    uint8_t num    = 0;
    BaseType_t ret = pdFALSE;

    debug_printf("task2 从队列queue接收数据...\r\n");
    
    for(;;)
    {
        ret = xQueueReceive(queue, &num, portMAX_DELAY);
        
        if (ret == pdPASS) 
        { 
            debug_printf("消息队列queue读取成功,值为:%d\r\n", num); 
        }
        else
        { 
            debug_printf("消息队列queue读取失败\r\n"); 
        }
    }
}

/**
 * @brief 任务主体：按键监控与发送 | 优先级：1 | 栈深度：128字
 */
static void task3(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_num = 0;
    BaseType_t ret  = pdFALSE;

    debug_printf("task3 运行中...\r\n");
    
    for(;;)
    {
        key_num = key_get_active();
        
        if (key_num != 0)
        {
            // RCS规范：发送至队列，若满则不阻塞
            ret = xQueueSend(queue, &key_num, 0);
            
            if (ret == pdPASS)
            { 
                debug_printf("消息队列queue发送成功,值为:%d\r\n", key_num); 
            }
            else
            { 
                debug_printf("消息队列queue发送失败!\r\n"); 
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20)); // 按键轮询周期优化为 20ms
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API                                                       */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    BaseType_t x_ret;

    /* 1. 资源初始化 */
    queue = xQueueCreate(3, sizeof(uint8_t));
    
    if (queue != NULL)
    { 
        debug_printf("消息队列创建成功!\r\n"); 
    }
    else 
    { 
        debug_printf("消息队列创建失败!\r\n");
        return; 
    }

    /* 2. 任务创建 (垂直对齐排版) */
    x_ret = xTaskCreate(task1, "task1", 128, NULL, 1, &task1_handle);
    if (x_ret != pdPASS) debug_printf("task1 创建失败\r\n");

    x_ret = xTaskCreate(task2, "task2", 128, NULL, 1, &task2_handle);
    if (x_ret != pdPASS) debug_printf("task2 创建失败\r\n");

    x_ret = xTaskCreate(task3, "task3", 128, NULL, 1, &task3_handle);
    if (x_ret != pdPASS) debug_printf("task3 创建失败\r\n");
}
