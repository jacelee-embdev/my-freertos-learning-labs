#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      task1_handle    = NULL;        // 按键扫描与事件分发任务句柄
static TaskHandle_t      task2_handle    = NULL;        // 事件监听与处理任务句柄
static QueueSetHandle_t  queueset_handle = NULL;        // 应用级队列集句柄
static QueueHandle_t     queue_handle    = NULL;        // 按键消息队列句柄
static SemaphoreHandle_t semphr_handle   = NULL;        // 按键事件二值信号量句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */

// 按键检测与事件分发任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t    numb    = 0;
    uint8_t    key_num = 0;
    BaseType_t ret     = 0;

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态分发系统事件 */
        if(key_num == 1)
        {
            ret = xQueueSend(queue_handle, &numb, portMAX_DELAY);
            if(ret == pdPASS) 
            { 
                debug_printf("队列写入数据成功\r\n"); 
                numb++;
            }
            else 
            {
                debug_printf("队列写入数据失败\r\n");
            }
        }
        else if(key_num == 2)
        {
            ret = xSemaphoreGive(semphr_handle);
            if(ret == pdPASS) 
            { 
                debug_printf("释放信号量成功\r\n");
            }
            else 
            {
                debug_printf("释放信号量失败\r\n");
            }
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (10ms 扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    QueueSetMemberHandle_t member_handle = NULL;
    uint8_t                numb          = 0;
    
    while(1)
    {
        /* 阶段 1: 阻塞等待队列集中的任何事件就绪 */
        member_handle = xQueueSelectFromSet(queueset_handle, portMAX_DELAY);
        
        /* 阶段 2: 鉴别唤醒源并处理对应事件 */
        if(member_handle == queue_handle)
        {
            xQueueReceive(queue_handle, &numb, portMAX_DELAY);
            debug_printf("获取到的队列数据为:%d\r\n", numb);
        }
        else if(member_handle == semphr_handle)
        {
            xSemaphoreTake(member_handle, portMAX_DELAY);
            debug_printf("获取信号量成功!\r\n");
        }
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    BaseType_t queue_addset_ret = 0;
    
    /* 阶段 1: 创建通信与同步机制资源 */
    queueset_handle = xQueueCreateSet(2);
    if(queueset_handle != NULL) { debug_printf("队列集创建成功!\r\n"); }
    else                        { debug_printf("队列集创建失败!\r\n"); }
    
    queue_handle = xQueueCreate(1, sizeof(uint8_t));
    if(queue_handle != NULL) { debug_printf("队列创建成功!\r\n"); }
    else                     { debug_printf("队列创建失败!\r\n"); }
    
    semphr_handle = xSemaphoreCreateBinary();
    if(semphr_handle != NULL) { debug_printf("二值信号量创建成功!\r\n"); }
    else                      { debug_printf("二值信号量创建失败!\r\n"); }
    
    /* 阶段 2: 将资源注册至队列集统筹管理 */
    queue_addset_ret = xQueueAddToSet(queue_handle, queueset_handle);
    if(queue_addset_ret == pdPASS) { debug_printf("队列添加到队列集成功!\r\n"); }
    else                           { debug_printf("队列添加到队列集失败!\r\n"); }
    
    queue_addset_ret = xQueueAddToSet(semphr_handle, queueset_handle);
    if(queue_addset_ret == pdPASS) { debug_printf("二值信号量添加到队列集成功!\r\n"); }
    else                           { debug_printf("二值信号量添加到队列集失败!\r\n"); }
    
    /* 阶段 3: 创建业务应用任务 */
    xTaskCreate(
        task1,
        "task1",
        128,
        NULL,
        1,
        &task1_handle
    );
    
    xTaskCreate(
        task2,
        "task2",
        128,
        NULL,
        1,
        &task2_handle
    );
}
