#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [配置参数区] 软件逻辑相关的参数阈值                                                */
/* ---------------------------------------------------------------------------------- */
#define TASK1_STACK_SIZE            128             // task1 栈大小 (字)
#define TASK2_STACK_SIZE            128             // task2 栈大小 (字)
#define TASK_DEFAULT_PRIORITY       1               // 默认任务优先级
#define TASK_ELEVATED_PRIORITY      5               // 提权后的任务优先级
#define TASK1_DELAY_MS              6000            // task1 运行周期
#define TASK2_SCAN_PERIOD_MS        10              // task2 扫描轮询周期

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */
static TaskHandle_t task1_handle = NULL;            // 测试任务 1 句柄
static TaskHandle_t task2_handle = NULL;            // 测试任务 2 句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */

// API 综合测试任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    UBaseType_t  current_priority   = 0;
    TaskHandle_t target_task_handle = NULL;
    TaskHandle_t self_task_handle   = NULL;

    /* 阶段 1: 任务优先级获取与设置测试 */
    current_priority = uxTaskPriorityGet(task2_handle);
    debug_printf("测试: 获取任务优先级 | task2 初始优先级: %d\r\n", (uint8_t)current_priority);
    
    vTaskPrioritySet(task2_handle, TASK_ELEVATED_PRIORITY);
    current_priority = uxTaskPriorityGet(task2_handle);
    debug_printf("测试: 设置任务优先级 | task2 修改后优先级: %d\r\n", (uint8_t)current_priority);
    
    /* 阶段 2: 任务句柄获取与挂起测试 (目标任务) */
    target_task_handle = xTaskGetHandle("task2");
    if (target_task_handle != NULL)
    {
        vTaskSuspend(target_task_handle);
        debug_printf("测试: 通过任务名获取句柄 | 成功获取并挂起 task2\r\n");
    }
    
    /* 阶段 3: 任务句柄获取与挂起测试 (自身任务) */
    self_task_handle = xTaskGetCurrentTaskHandle();
    if (self_task_handle != NULL)
    {
        debug_printf("测试: 获取当前任务句柄 | 即将挂起 task1 自身\r\n");
        vTaskSuspend(self_task_handle);
        debug_printf("异常: 当前任务已被挂起，若打印此信息说明挂起自身失败或被意外恢复!\r\n");
    }
    
    /* 阶段 4: 挂起恢复后的周期性运行指示 */
    while (1)
    {
        debug_printf("状态: task1 正在运行...\r\n");
        vTaskDelay(pdMS_TO_TICKS(TASK1_DELAY_MS)); 
    }
}

// 按键扫描与事件分发任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t key_num = 0;
    
    while (1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态分发系统事件 */
        if (key_num == 1)
        {
            debug_printf("事件: 检测到 KEY 1 触发\r\n");
        }
        else if (key_num == 2)
        {
            debug_printf("事件: 检测到 KEY 2 触发\r\n");
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(TASK2_SCAN_PERIOD_MS)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    /* 创建测试任务 1 */
    xTaskCreate(
        task1,
        "task1",
        TASK1_STACK_SIZE,
        NULL,
        TASK_DEFAULT_PRIORITY,
        &task1_handle
    );
    
    /* 创建测试任务 2 */
    xTaskCreate(
        task2,
        "task2",
        TASK2_STACK_SIZE,
        NULL,
        TASK_DEFAULT_PRIORITY,
        &task2_handle
    );
}
