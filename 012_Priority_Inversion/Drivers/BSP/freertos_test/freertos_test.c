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

static TaskHandle_t      task_low_handle       = NULL;              // 低优先级任务句柄
static TaskHandle_t      task_middle_handle    = NULL;              // 中优先级任务句柄
static TaskHandle_t      task_high_handle      = NULL;              // 高优先级任务句柄
static SemaphoreHandle_t binary_sem_handle     = NULL;              // 二值信号量句柄 (用于按键事件同步)

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

// 低优先级任务：获取信号量并执行耗时操作
static void task_low(void *pvParameters)
{
    (void)pvParameters; 

    while(1)
    {
        debug_printf("[Low] 尝试获取信号量...\r\n");
        xSemaphoreTake(binary_sem_handle, portMAX_DELAY);
        debug_printf("[Low] 成功获取信号量! 开始处理数据...\r\n");
        
        /* 阶段 1: 模拟长时间占用资源的耗时操作 (霸占 CPU) */
        delay_ms(3000); 
        
        debug_printf("[Low] 数据处理完毕，释放信号量...\r\n");
        xSemaphoreGive(binary_sem_handle);
        
        /* 阶段 2: 任务休眠，出让 CPU 开启下一轮 */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 中优先级任务：定期唤醒并抢占低优先级任务
static void task_middle(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        /* 阶段 1: 初始休眠，确保 Low 先拿到信号量且 High 已阻塞等待 */
        vTaskDelay(pdMS_TO_TICKS(1000));

        debug_printf("[Middle] 唤醒! 开始执行繁重的计算任务，强行抢占 Low 任务...\r\n");
        
        /* 阶段 2: 模拟极度耗时的任务 (霸占 CPU) */
        delay_ms(4000); 
        
        debug_printf("[Middle] 繁重任务执行完毕，让出 CPU...\r\n");
        
        /* 阶段 3: 休息较长时间，方便串口观察完整一轮现象 */
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

// 高优先级任务：尝试获取被低优先级任务占用的信号量
static void task_high(void *pvParameters)
{
    (void)pvParameters; 

    while(1)
    {
        /* 阶段 1: 初始休眠，故意让 Low 任务先跑并拿到信号量 */
        vTaskDelay(pdMS_TO_TICKS(500)); 

        debug_printf("[High] 唤醒! 尝试获取信号量执行紧急任务...\r\n");
        
        /* 阶段 2: 尝试获取信号量 (此时进入阻塞状态) */
        xSemaphoreTake(binary_sem_handle, portMAX_DELAY);

        debug_printf("[High] 成功获取信号量! 紧急任务执行中...\r\n");
        delay_ms(500); 

        debug_printf("[High] 紧急任务处理完毕，释放信号量...\r\n");
        xSemaphoreGive(binary_sem_handle);

        /* 阶段 3: 任务休眠 */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (负责统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建 OS 通信资源 (二值信号量) */
    binary_sem_handle = xSemaphoreCreateBinary();
    if(binary_sem_handle != NULL) 
    {
        printf("二值信号量创建成功!\r\n");
    }
    else 
    {
        printf("二值信号量创建失败!\r\n");
    }

    xSemaphoreGive(binary_sem_handle);
    
    /* 阶段 2: 创建硬件外设控制与监控任务 */
    xTaskCreate(
        task_low,                                                   // 任务入口函数
        "task_low",                                                 // 任务名称
        128,                                                        // 堆栈深度(单位:字)
        NULL,                                                       // 任务入口参数
        1,                                                          // 任务优先级
        &task_low_handle                                            // 任务句柄
    );
    
    xTaskCreate(
        task_middle,
        "task_middle",
        128,
        NULL,
        2,
        &task_middle_handle
    );
    
    xTaskCreate(
        task_high,
        "task_high",
        128,
        NULL,
        3,
        &task_high_handle
    );
}
