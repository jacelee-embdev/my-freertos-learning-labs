#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led.h"
#include "key.h"
#include "uart1.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的内存缓冲区与 OS 对象                                */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t  task1_handle = NULL; // 任务 1 句柄
static TaskHandle_t  task2_handle = NULL; // 任务 2 句柄
static TaskHandle_t  task3_handle = NULL; // 任务 3 句柄
static QueueHandle_t queue        = NULL; // 消息队列句柄

static char messge[1000] = {
"The universe is a dark forest. Every civilization is an armed hunter \
stalking through the trees like a ghost, gently pushing aside branches \
that block the path and trying to tread without sound. Even breathing is \
done with care. The hunter has to be careful, because everywhere in the forest \
are stealthy hunters like him."
};

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief 任务主体：LED 状态指示 | 优先级：1 | 栈深度：128字
 */
static void task1(void *pvParameters)
{
    (void)pvParameters;
    debug_printf("task1 运行中...\r\n");
    
    while(1)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief 任务主体：消息队列接收 | 优先级：1 | 栈深度：128字
 */
static void task2(void *pvParameters)
{
    (void)pvParameters;
    char *p_receive_buf = &messge[0];
    BaseType_t ret      = pdFALSE;

    debug_printf("task2 从队列queue接收数据...\r\n");
    while(1)
    {
        // 阻塞等待队列信号
        ret = xQueueReceive(queue, &p_receive_buf, portMAX_DELAY);
        
        if (ret == pdPASS) 
        {
            debug_printf("消息队列queue读取成功,值为:%s\r\n", p_receive_buf); 
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
    char *p_send_buf = &messge[0];
    BaseType_t ret   = pdFALSE;

    debug_printf("task3 运行中...\r\n");
    while(1)
    {
        key_num = key_get_active();
        
        if (key_num != 0)
        {
            // 发送数据至队列，若满则立即返回
            ret = xQueueSend(queue, &p_send_buf, 0);
            
            if (ret == pdPASS)
            {
                debug_printf("消息队列queue发送成功,值为:%s\r\n", p_send_buf); 
            }
            else
            {
                debug_printf("消息队列queue发送失败!\r\n"); 
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API                                                       */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    BaseType_t ret;

    /* 1. 资源初始化 (根据核定后的 TeriSink 类型创建) */
    queue = xQueueCreate(3, sizeof(TeriSink));
    
    if (queue != NULL)
    {
        debug_printf("消息队列创建成功!\r\n"); 
    }
    else 
    {
        debug_printf("消息队列创建失败!\r\n");
        return; 
    }

    /* 2. 并发任务创建 (垂直对齐) */
    ret = xTaskCreate(task1, "task1", 128, NULL, 1, &task1_handle);
    if (ret != pdPASS) debug_printf("task1 创建失败\r\n");

    ret = xTaskCreate(task2, "task2", 128, NULL, 1, &task2_handle);
    if (ret != pdPASS) debug_printf("task2 创建失败\r\n");

    ret = xTaskCreate(task3, "task3", 128, NULL, 1, &task3_handle);
    if (ret != pdPASS) debug_printf("task3 创建失败\r\n");
}
