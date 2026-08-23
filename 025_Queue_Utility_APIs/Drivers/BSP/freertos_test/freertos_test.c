#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "led.h"
#include "key.h"
#include "uart1.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部 OS 资源隔离管控                                          */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t  task1_handle = NULL;
static TaskHandle_t  task2_handle = NULL;
static QueueHandle_t queue        = NULL; 

/* ---------------------------------------------------------------------------------- */
/* [任务主体] 业务逻辑实现层                                                          */
/* ---------------------------------------------------------------------------------- */

// LED 心跳指示任务
static void task1(void *pvParameters)
{
    (void)pvParameters; 
    
    debug_printf("task1 运行中...\r\n");
    
    for(;;)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 按键事件采集与分发任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t     key_num      = 0;
    uint8_t     recv_num     = 0;
    BaseType_t  os_ret       = pdFALSE;
    UBaseType_t queue_space  = 0;
    UBaseType_t queue_filled = 0;

    debug_printf("task2 运行中...\r\n");
    
    for(;;)
    {
        /* 阶段 1: 轮询获取按键物理状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键值执行分支路由 */
        if (key_num == 1)
        {
            os_ret = xQueueSend(queue, &key_num, 0);
            
            if (os_ret == pdPASS)
            {
                debug_printf("消息队列queue发送成功,值为:%d\r\n", key_num);
                queue_space = uxQueueSpacesAvailable(queue);
                debug_printf("队列剩余容量:%ld\r\n", queue_space);
                
                if (queue_space == 0)
                {
                    os_ret = xQueueReset(queue);
                    if (os_ret == pdPASS)
                    {
                        debug_printf("队列清空成功!\r\n");
                    }
                }
            }
            else
            {
                debug_printf("消息队列queue发送失败!\r\n"); 
            }
        }
        else if (key_num == 2)
        {
            os_ret = xQueueReceive(queue, &recv_num, pdMS_TO_TICKS(10));
            
            if (os_ret == pdPASS) 
            {
                debug_printf("消息队列queue读取成功,值为:%d\r\n", recv_num); 
                queue_filled = uxQueueMessagesWaiting(queue);
                debug_printf("队列剩余消息数:%ld\r\n", queue_filled);
            }
            else
            {
                debug_printf("消息队列queue读取失败\r\n"); 
            }
        }
        
        /* 阶段 3: 释放 CPU 使用权 */
        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外暴露的初始化与启动函数                                            */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    BaseType_t os_ret = pdFALSE;

    /* 1. 核心通信组件初始化 */
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

    /* 2. 业务任务创建 */
    os_ret = xTaskCreate(task1, "task1", 128, NULL, 1, &task1_handle);
    if (os_ret != pdPASS) 
    {
        debug_printf("task1 创建失败\r\n");
    }

    os_ret = xTaskCreate(task2, "task2", 128, NULL, 1, &task2_handle);
    if (os_ret != pdPASS)
    {
        debug_printf("task2 创建失败\r\n");
    }
}
