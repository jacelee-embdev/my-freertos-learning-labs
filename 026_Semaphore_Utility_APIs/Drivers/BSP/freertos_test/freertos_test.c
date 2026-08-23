#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "semphr.h"
#include "delay.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      task_low_handle     = NULL;                // 低优先级任务句柄
static TaskHandle_t      task_middle_handle  = NULL;                // 中优先级任务句柄
static TaskHandle_t      task_high_handle    = NULL;                // 高优先级任务句柄
static SemaphoreHandle_t test_mutex_handle   = NULL;                // 测试用互斥信号量句柄
static TaskHandle_t      mutex_holder_handle = NULL;                // 互斥量持有者句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] 优先级继承实验任务逻辑实现                                              */
/* ---------------------------------------------------------------------------------- */

// 低优先级任务
static void task_low(void *pvParameters)
{
    (void)pvParameters; 

    while(1)
    {
        /* 阶段 1: 申请临界资源 */
        debug_printf("[Low] 尝试获取信号量...\r\n");
        xSemaphoreTake(test_mutex_handle, portMAX_DELAY);
        debug_printf("[Low] 成功获取信号量! 开始处理数据...\r\n");
        
        /* 阶段 2: 模拟长时间占用资源的计算密集型操作 */
        delay_ms(3000); 
        
        /* 阶段 3: 释放资源并进入 OS 级阻塞 */
        debug_printf("[Low] 数据处理完毕，释放信号量...\r\n");
        xSemaphoreGive(test_mutex_handle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 中优先级任务
static void task_middle(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        /* 阶段 1: 初始延时，确保时序上晚于 Low 任务唤醒 */
        vTaskDelay(pdMS_TO_TICKS(300));

        /* 阶段 2: 唤醒并执行剥夺式抢占的模拟计算 */
        debug_printf("[Middle] 唤醒! 开始执行繁重的计算任务，强行抢占 Low 任务...\r\n");
        delay_ms(4000); 
        
        /* 阶段 3: 任务完成，出让 CPU 调度权 */
        debug_printf("[Middle] 繁重任务执行完毕，让出 CPU...\r\n");
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

// 高优先级任务
static void task_high(void *pvParameters)
{
    (void)pvParameters; 

    while(1)
    {
        /* 阶段 1: 深度延时，等待 Low 和 Middle 均已切入运行态后介入 */
        vTaskDelay(pdMS_TO_TICKS(1000));
        debug_printf("[High] 唤醒! 尝试获取信号量执行紧急任务...\r\n");
        
        mutex_holder_handle = xSemaphoreGetMutexHolder(test_mutex_handle);
        
        if(mutex_holder_handle != NULL)
        {
            debug_printf("互斥量当前有任务[%s]持有\r\n", pcTaskGetName(mutex_holder_handle)); 
        }
        else
        {
            debug_printf("互斥量未被锁定.\r\n");
        }
        
        /* 阶段 2: 阻塞式请求 Mutex，触发底层优先级继承机制 */
        xSemaphoreTake(test_mutex_handle, portMAX_DELAY);
        debug_printf("[High] 成功获取信号量! 紧急任务执行中...\r\n");
        delay_ms(500); 

        /* 阶段 3: 归还 Mutex，恢复系统原始优先级状态 */
        debug_printf("[High] 紧急任务处理完毕，释放信号量...\r\n");
        xSemaphoreGive(test_mutex_handle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建 OS 通信资源 (Mutex) */
    test_mutex_handle = xSemaphoreCreateMutex();
    
    if(test_mutex_handle != NULL) 
    {
        printf("互斥信号量创建成功!\r\n");
    }
    else 
    {
        printf("互斥信号量创建失败!\r\n");
    }

    /* 阶段 2: 注册并激活各优先级实验任务 */
    xTaskCreate(
        task_low,                                                           // 任务入口函数
        "task_low",                                                         // 任务名称
        128,                                                                // 堆栈深度(字)
        NULL,                                                               // 任务入口参数
        1,                                                                  // 任务优先级
        &task_low_handle                                                    // 任务句柄
    );
    
    xTaskCreate(
        task_middle,                                                        // 任务入口函数
        "task_middle",                                                      // 任务名称
        128,                                                                // 堆栈深度(字)
        NULL,                                                               // 任务入口参数
        2,                                                                  // 任务优先级
        &task_middle_handle                                                 // 任务句柄
    );
    
    xTaskCreate(
        task_high,                                                          // 任务入口函数
        "task_high",                                                        // 任务名称
        128,                                                                // 堆栈深度(字)
        NULL,                                                               // 任务入口参数
        3,                                                                  // 任务优先级
        &task_high_handle                                                   // 任务句柄
    );
}
