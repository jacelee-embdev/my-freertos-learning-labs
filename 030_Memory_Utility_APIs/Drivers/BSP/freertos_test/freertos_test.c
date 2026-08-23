#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t task1_handle      = NULL;                       // 按键扫描与事件分发任务句柄
static TaskHandle_t task2_handle      = NULL;                       // 事件监听与处理任务句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 按键扫描与事件分发任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t  key_num = 0;
    uint8_t *buf     = NULL;

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键值执行内存的申请或释放操作 */
        if(key_num == 1)
        {
            buf = pvPortMalloc(30);
            if(buf != NULL)
            {
                debug_printf("[task1] 内存申请成功!\r\n");
            }
            else 
            {
                debug_printf("[task1] 内存申请失败!\r\n");
            }
        }
        else if(key_num == 2)
        {
            if(buf != NULL)
            {
                vPortFree(buf);
                buf = NULL;                                         // 释放后置空，防止产生野指针
                debug_printf("[task1] 内存释放成功!\r\n");
            }
            else
            {
                debug_printf("[task1] 内存释放失败!请勿重复释放!\r\n");
            }
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    HeapStats_t stats;

    while(1)
    {
        /* 阶段 1: 获取当前堆内存状态详情 */
        vPortGetHeapStats(&stats);
        debug_printf("内存使用情况:\r\n");
        debug_printf("当前空闲:%d\n最大连续块:%d\n分配次数:%d\n释放次数:%d\r\n",
                     stats.xAvailableHeapSpaceInBytes,
                     stats.xSizeOfLargestFreeBlockInBytes,
                     stats.xNumberOfSuccessfulAllocations,
                     stats.xNumberOfSuccessfulFrees);
        
        /* 阶段 2: 打印当前系统剩余堆内存大小 */
        debug_printf("[task2] 剩余空闲内存大小:%d\r\n", xPortGetFreeHeapSize());
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建按键扫描业务任务 */
    xTaskCreate(task1,
                "task1",
                128,
                NULL,
                1,
                &task1_handle);
    
    /* 阶段 2: 创建事件处理业务任务 */
    xTaskCreate(task2,
                "task2",
                128,
                NULL,
                1,
                &task2_handle);
}
