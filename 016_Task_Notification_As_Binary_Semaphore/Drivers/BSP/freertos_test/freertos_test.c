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
/* [配置参数区] 软件逻辑相关的参数阈值与宏定义                                        */
/* ---------------------------------------------------------------------------------- */

#define TASK1_STACK_SIZE              (128)                                 // task1 任务堆栈大小
#define TASK1_PRIO                    (1)                                   // task1 任务优先级
#define TASK2_STACK_SIZE              (128)                                 // task2 任务堆栈大小
#define TASK2_PRIO                    (1)                                   // task2 任务优先级


/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t task1_handle      = NULL;                               // 按键扫描与事件分发任务句柄
static TaskHandle_t task2_handle      = NULL;                               // 事件监听与处理任务句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 按键检测与事件分发任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t key_num = 0;

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态分发系统事件 */
        if(key_num == 1)
        {
            debug_printf("[task1] 任务通知模拟二值信号量释放!\r\n");
            xTaskNotifyGive(task2_handle);
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint32_t recv = 0;
    
    while(1)
    {
        /* 阶段 1: 阻塞等待任务通知 (模拟获取二值信号量) */
        recv = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        /* 阶段 2: 验证接收结果并执行相应逻辑 */
        if(recv != 0)
        {
            debug_printf("[task2] 任务通知接收成功,模拟获取二值信号量释放!\r\n");
        }
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
                TASK1_STACK_SIZE,
                NULL,
                TASK1_PRIO,
                &task1_handle);
    
    /* 阶段 2: 创建事件处理业务任务 */
    xTaskCreate(task2,
                "task2",
                TASK2_STACK_SIZE,
                NULL,
                TASK2_PRIO,
                &task2_handle);
}
