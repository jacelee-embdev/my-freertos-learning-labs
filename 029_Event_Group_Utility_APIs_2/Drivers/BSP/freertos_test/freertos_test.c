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

#define EVENTBIT_0                              (1 << 0)                // 按键 1 触发事件标志位
#define EVENTBIT_1                              (1 << 1)                // 按键 2 触发事件标志位
#define EVENTBIT_2                              (1 << 2)                // 主任务事件标志位
#define INIT_BIT                                (EVENTBIT_0 | EVENTBIT_1 | EVENTBIT_2) // 初始化同步掩码

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t       task1_handle          = NULL;                 // 任务 1 句柄
static TaskHandle_t       task2_handle          = NULL;                 // 任务 2 句柄
static TaskHandle_t       task_main_handle      = NULL;                 // 主任务句柄
static EventGroupHandle_t eventgroup_handle     = NULL;                 // 事件标志组句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
/* ---------------------------------------------------------------------------------- */

static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t key_num = 0;

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态执行系统级事件同步 */
        if(key_num == 1)
        {
            xEventGroupSync(eventgroup_handle, EVENTBIT_0, INIT_BIT, portMAX_DELAY);
            debug_printf("模块1初始化完毕\r\n");
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (10ms 扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t key_num = 0;

    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 根据按键状态执行系统级事件同步 */
        if(key_num == 2)
        {
            xEventGroupSync(eventgroup_handle, EVENTBIT_1, INIT_BIT, portMAX_DELAY);
            debug_printf("模块2初始化完毕\r\n");
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (10ms 扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

static void task_main(void *pvParameters)
{
    (void)pvParameters;
    
    while(1)
    {
        /* 阶段 1: 参与系统级事件同步，等待所有前置任务就绪 */
        xEventGroupSync(eventgroup_handle, EVENTBIT_2, INIT_BIT, portMAX_DELAY);
        debug_printf("所有模块初始化完毕\r\n");
        
        /* 阶段 2: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 初始化系统测试任务 (统筹创建各个子任务)
void app_freertos_test_init(void)
{
    /* 阶段 1: 创建系统通信与同步资源 */
    eventgroup_handle = xEventGroupCreate();
    if(eventgroup_handle != NULL) 
    { 
        debug_printf("事件标志组创建成功!\r\n"); 
    }
    else                          
    { 
        debug_printf("事件标志组创建失败!\r\n"); 
    }
    
    /* 阶段 2: 注册并启动相关业务与同步任务 */
    xTaskCreate(task1,
                "task1",
                128,
                NULL,
                1,
                &task1_handle);
    
    xTaskCreate(task2,
                "task2",
                128,
                NULL,
                1,
                &task2_handle);
    
    xTaskCreate(task1,
                "task1",
                128,
                NULL,
                1,
                &task1_handle);
                
    xTaskCreate(task2,
                "task2",
                128,
                NULL,
                1,
                &task2_handle);
                
    xTaskCreate(task_main,
                "task_main",
                128,
                NULL,
                1,
                &task_main_handle);
}
