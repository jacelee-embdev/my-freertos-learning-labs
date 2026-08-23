#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [配置参数区] 软件逻辑相关的参数阈值宏定义                                          */
/* ---------------------------------------------------------------------------------- */

#define EVENTBIT_0                  (1 << 0)
#define EVENTBIT_1                  (1 << 1)

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t task1_handle = NULL;                            // 按键扫描与事件分发任务句柄
static TaskHandle_t task2_handle = NULL;                            // 事件监听与处理任务句柄

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
        
        /* 阶段 2: 触发有效按键时递增任务通知值 */
        if(key_num == 1)
        {
            debug_printf("[task1] 将bit0位置为1\r\n");
            xTaskNotify(task2_handle, EVENTBIT_0, eSetBits);
        }
        else if(key_num == 2)
        {
            debug_printf("[task1] 将bit1位置为1\r\n");
            xTaskNotify(task2_handle, EVENTBIT_1, eSetBits);
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint32_t new_events = 0;
    uint32_t acc_events = 0;
    
    while(1)
    {
        /* 阶段 1: 阻塞等待按键事件通知 */
        xTaskNotifyWait(0x00,                                       // 进入等待前，不清零
                        0xFFFFFFFF,                                 // 成功接收并退出前，清空所有位
                        &new_events,                                // 保存接收到的 32 位通知值
                        portMAX_DELAY);                             // 最大阻塞等待时间：死等
                        
        /* 阶段 2: 校验并累加接收到的事件标志位 */
        if(new_events & EVENTBIT_0)
        {
            acc_events |= EVENTBIT_0;
        }
        else if(new_events & EVENTBIT_1)
        {
            acc_events |= EVENTBIT_1;
        }
        
        /* 阶段 3: 检查多事件组合条件是否达成 */
        if(acc_events == (EVENTBIT_0 | EVENTBIT_1))
        {
            debug_printf("[task2] 任务通知模拟事件标志组接收成功，值：%d\r\n", acc_events);
            acc_events = 0;
        }
        
        /* 阶段 4: 控制执行节拍 */
        vTaskDelay(pdMS_TO_TICKS(1000));
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
