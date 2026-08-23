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

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t       task1_handle          = NULL;                 // 按键扫描与事件分发任务句柄
static TaskHandle_t       task2_handle          = NULL;                 // 事件监听与处理任务句柄
static EventGroupHandle_t eventgroup_handle     = NULL;                 // 按键事件标志组句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体] RTOS 任务逻辑实现                                                       */
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
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_0);
            debug_printf("设置事件标志组的bit0为1\r\n");
        }
        else if(key_num == 2)
        {
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_1);
            debug_printf("设置事件标志组的bit1为1\r\n");
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (10ms 扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    EventBits_t bits;

    while(1)
    {
        /* 阶段 1: 获取当前系统事件标志组状态 */
        bits = xEventGroupGetBits(eventgroup_handle);
        debug_printf("当前事件标志位为:\r\n");
        
        /* 阶段 2: 解析事件位并执行对应的业务逻辑 */
        if(bits & EVENTBIT_0)
        {
            debug_printf("按键1被按下\r\n");
        }
        
        if(bits & EVENTBIT_1)
        {
            debug_printf("按键2被按下\r\n");
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    
    /* 阶段 2: 创建按键扫描业务任务 */
    xTaskCreate(task1,
                "task1",
                128,
                NULL,
                1,
                &task1_handle);
    
    /* 阶段 3: 创建事件处理业务任务 */
    xTaskCreate(task2,
                "task2",
                128,
                NULL,
                1,
                &task2_handle);
}
