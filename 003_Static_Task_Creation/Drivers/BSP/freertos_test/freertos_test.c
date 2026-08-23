#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

// 任务句柄：用于任务级的调度与控制 (如挂起、恢复、删除任务)
static TaskHandle_t led1_task_handle        = NULL;
static TaskHandle_t led2_task_handle        = NULL;
static TaskHandle_t key_monitor_task_handle = NULL;

// 任务堆栈：静态分配的任务运行内存空间 (存放局部变量与现场上下文，单位：字)
static StackType_t  led1_task_stack[128];
static StackType_t  led2_task_stack[128];
static StackType_t  key_monitor_task_stack[128];

// 任务控制块 (TCB)：OS 核心数据结构，存储任务状态、优先级、栈顶指针等信息
static StaticTask_t led1_task_tcb;
static StaticTask_t led2_task_tcb;
static StaticTask_t key_monitor_task_tcb;

// 空闲任务配置：系统 CPU 空闲时自动运行的最低优先级任务
static StaticTask_t idle_task_tcb;
static StackType_t  idle_task_stack[configMINIMAL_STACK_SIZE];

// 软件定时器任务配置：负责处理 FreeRTOS 软件定时器回调逻辑
static StaticTask_t timer_task_tcb;
static StackType_t  timer_task_stack[configTIMER_TASK_STACK_DEPTH];

/* ---------------------------------------------------------------------------------- */
/* [系统级钩子函数] FreeRTOS 静态分配所需的内存接口                                   */
/* ---------------------------------------------------------------------------------- */

/* * 空闲任务内存分配
 * * [内存分配的指针层级映射说明]
 * (a) 变量本体：idle_task_tcb / idle_task_stack。这是用户态静态分配的物理内存实体。
 * (b) 一级指针：pxIdleTaskTCBBuffer / pxIdleTaskStackBuffer。它们是隐藏在 FreeRTOS 
 * 内核中的指针变量。我们在函数内部执行的 *ppx... 解引用操作，本质上
 * 就是直接给内核中的这些一级指针赋值，让内核顺藤摸瓜找到我们的本体。
 * (c) 二级指针：ppxIdleTaskTCBBuffer / ppxIdleTaskStackBuffer。即本函数的入参，
 * 它们存放的是内核中一级指针的物理地址。
 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE * puxIdleTaskStackSize )
{
    // 对二级指针解引用，实质是为内核的一级指针 (pxIdleTaskTCBBuffer) 赋本体地址
    *ppxIdleTaskTCBBuffer   = &idle_task_tcb;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *puxIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

// 软件定时器内存分配
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     configSTACK_DEPTH_TYPE * puxTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer   = &timer_task_tcb;
    *ppxTimerTaskStackBuffer = timer_task_stack;
    *puxTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
}

/* ---------------------------------------------------------------------------------- */
/* [任务主体(Task)] RTOS 任务逻辑实现                                                 */
/* ---------------------------------------------------------------------------------- */

// 任务主体：LED1 状态慢速闪烁指示 | 优先级：1 | 栈深度：128字
static void led1_task(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        led1_on();
        vTaskDelay(pdMS_TO_TICKS(1000));
        led1_off();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// 任务主体：LED2 状态快速闪烁指示 | 优先级：1 | 栈深度：128字
static void led2_task(void *pvParameters)
{
    (void)pvParameters;

    while(1)
    {
        led2_on();
        vTaskDelay(pdMS_TO_TICKS(500));
        led2_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// 任务主体：按键轮询监控与任务动态管理 | 优先级：1 | 栈深度：128字
static void key_monitor_task(void *pvParameters)
{
    (void)pvParameters;
    uint8_t key_num = 0;

    while(1)
    {
        key_num = key_get_active();
        
        if(key_num == 1)
        {
            if(led1_task_handle != NULL)
            {
                printf("删除 led1_task\r\n");
                vTaskDelete(led1_task_handle);
                led1_task_handle = NULL;
            }
            else
            {
                printf("led1_task 已经不存在了，无需重复删除\r\n");
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void)
{
    // 首次创建静态任务，保留参数说明模板以供后续参考
    led1_task_handle = xTaskCreateStatic(
        led1_task,                  // 1. 任务入口函数 (操作手册：规定任务逻辑)
        "led1_task",                // 2. 任务名 (调试标签：方便在控制台识别)
        128,                        // 3. 堆栈深度 (办公室场地大小)
        NULL,                       // 4. 任务入口参数 (传给手册的初始数据)
        1,                          // 5. 任务优先级
        led1_task_stack,            // 6. 堆栈数组指针 (办公室门牌号:指向 RAM 运行空间)
        &led1_task_tcb              // 7. TCB 结构体指针 (员工档案地址:任务的大总管)
    );
    
    led2_task_handle = xTaskCreateStatic(
        led2_task,
        "led2_task",
        128,
        NULL,
        1,
        led2_task_stack,
        &led2_task_tcb
    );
    
    key_monitor_task_handle = xTaskCreateStatic(
        key_monitor_task,
        "key_monitor",
        128,
        NULL,
        1,
        key_monitor_task_stack,
        &key_monitor_task_tcb
    );
}
