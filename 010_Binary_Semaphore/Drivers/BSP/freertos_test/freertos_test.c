#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include "semphr.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t      led1_task_handle        = NULL;            // LED1 控制任务句柄
static TaskHandle_t      key_monitor_task_handle = NULL;            // 按键监控任务句柄
static SemaphoreHandle_t binary_sem_handle       = NULL;            // 二值信号量句柄 (用于按键事件同步)

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

// LED1 状态慢速闪烁指示 | 优先级：1 | 栈深度：128字
static void led1_task(void *pvParameters)
{
    debug_printf("led1_task 运行中...\r\n");
    (void)pvParameters; 

    while(1)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000));
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 按键轮询监控与任务动态管理 | 优先级：1 | 栈深度：128字
static void key_monitor_task(void *pvParameters)
{
    uint8_t    key_num  = 0;
    BaseType_t ret_give = pdFAIL;
    BaseType_t ret_take = pdFAIL;

    debug_printf("key_monitor_task 运行中...\r\n");
    (void)pvParameters;

    while(1)
    {
        /* 阶段 1: 获取当前激活的按键键值 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据键值执行信号量的释放或获取操作 */
        if(key_num == 1)
        {
            ret_give = xSemaphoreGive(binary_sem_handle);
            if(ret_give == pdPASS)
            {
                printf("二值信号量释放成功!\r\n");
            }
            else                        
            {
                printf("二值信号量释放失败!\r\n");
            }
        }
        else if(key_num == 2)
        {
            ret_take = xSemaphoreTake(binary_sem_handle, pdMS_TO_TICKS(100));
            if(ret_take == pdPASS)   
            {
                printf("二值信号量获取成功!\r\n");
            }
            else                     
            {
                printf("二值信号量获取失败!\r\n");
            }
        }
        
        /* 阶段 3: 阻塞延时，让出 CPU 资源 (10ms 轮询周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
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

    /* 阶段 2: 创建硬件外设控制与监控任务 */
    xTaskCreate(
        led1_task,                  // 任务入口函数
        "led1_task",                // 任务名称
        128,                        // 堆栈深度(单位:字)
        NULL,                       // 任务入口参数
        1,                          // 任务优先级
        &led1_task_handle           // 任务句柄
    );
    
    xTaskCreate(
        key_monitor_task,
        "key_monitor_task",
        128,
        NULL,
        1,
        &key_monitor_task_handle
    );
}
