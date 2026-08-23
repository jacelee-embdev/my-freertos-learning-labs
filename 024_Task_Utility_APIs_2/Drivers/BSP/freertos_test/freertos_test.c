#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */
static TaskHandle_t task1_handle = NULL;                    // 测试任务 1 句柄
static TaskHandle_t task2_handle = NULL;                    // 测试任务 2 句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */

// API 综合测试任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    char          task_list_buf[128];
    UBaseType_t   total_task_num;
    TaskStatus_t *p_task_status_array;
    uint32_t      sys_total_run_time;
    TaskHandle_t  target_task_handle;
    TaskStatus_t  target_task_info;
    
    /* 阶段 1: 获取并打印系统任务列表 */
    vTaskList(task_list_buf);
    debug_printf("任务名称\t状态\t优先级\t堆栈\t任务编号\r\n");
    debug_printf("%s\r\n", task_list_buf);
    debug_printf("X-运行态  R-就绪态  B-阻塞态  S-挂起态  D--删除态\r\n\r\n");
    
    /* 阶段 2: 获取并打印所有任务的运行状态时间 */
    total_task_num      = uxTaskGetNumberOfTasks();
    p_task_status_array = pvPortMalloc(total_task_num * sizeof(TaskStatus_t));
    
    if (p_task_status_array != NULL)
    {
        uxTaskGetSystemState(p_task_status_array, total_task_num, &sys_total_run_time);
        debug_printf("任务名称\t优先级\t运行态时间\r\n");
        for (int i = 0; i < total_task_num; i++)
        {
            printf("%s\t\t%ld\t%d\r\n", p_task_status_array[i].pcTaskName,
                                        p_task_status_array[i].uxCurrentPriority,
                                        p_task_status_array[i].ulRunTimeCounter);
        }
        vPortFree(p_task_status_array);                     // 释放动态分配的内存，防止内存泄漏
    }
    debug_printf("\r\n");
    
    /* 阶段 3: 获取并打印指定任务的详细信息 */
    target_task_handle = xTaskGetHandle("task1");
    if (target_task_handle != NULL)
    {
        vTaskGetInfo(target_task_handle, &target_task_info, pdTRUE, eInvalid);
        debug_printf("任务名称\t优先级\t运行态时间\t任务栈高水位线\r\n");
        printf("%s\t\t%ld\t%d\t%u\r\n\r\n", target_task_info.pcTaskName,
                                            target_task_info.uxCurrentPriority,
                                            target_task_info.ulRunTimeCounter,
                                            target_task_info.usStackHighWaterMark);
    }
    
    /* 阶段 4: 周期性运行指示 */
    while (1)
    {
        debug_printf("状态: task1 正在运行...\r\n");
        vTaskDelay(pdMS_TO_TICKS(6000)); 
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
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 创建测试任务 1 */
    xTaskCreate(
        task1,
        "task1",
        128,
        NULL,
        1,
        &task1_handle
    );
    
    /* 创建测试任务 2 */
    xTaskCreate(
        task2,
        "task2",
        128,
        NULL,
        1,
        &task2_handle
    );
}
