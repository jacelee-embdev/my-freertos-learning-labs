#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "key.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的 OS 资源句柄                                        */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t task1_handle = NULL;                    // 按键扫描与事件分发任务句柄
static TaskHandle_t task2_handle = NULL;                    // 事件监听与处理任务句柄

/* ---------------------------------------------------------------------------------- */
/* [任务主体区] RTOS 任务逻辑实现                                                     */
/* ---------------------------------------------------------------------------------- */

// 按键检测与事件分发任务
static void task1(void *pvParameters)
{
    (void)pvParameters;
    
    uint8_t  key_num      = 0;
    uint32_t notify_count = 0;
    
    while(1)
    {
        /* 阶段 1: 获取按键活跃状态 */
        key_num = key_get_active();
        
        /* 阶段 2: 触发有效按键时递增任务通知值 (模拟释放计数型信号量) */
        if(key_num == 1)
        {
            debug_printf("[task1] 任务通知模拟邮箱发送，值：%d\r\n", notify_count);
            xTaskNotify(task2_handle, notify_count, eSetValueWithOverwrite);
            notify_count++;
        }
        
        /* 阶段 3: 阻塞让出 CPU 资源 (扫描周期) */
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}

// 事件监听与处理任务
static void task2(void *pvParameters)
{
    (void)pvParameters;
    
    uint32_t received_notify_val = 0;
    
    while(1)
    {
        /* 阶段 1: 阻塞等待按键事件通知 */
        xTaskNotifyWait(0x00,                           // 进入等待前, 不清零
                        0xFFFFFFFF,                     // 成功接收并退出前, 清空所有位
                        &received_notify_val,           // 保存接收到的 32 位通知值
                        portMAX_DELAY);                 // 最大阻塞等待时间: 死等
                        
        /* 阶段 2: 响应事件并执行业务逻辑 */
        debug_printf("[task2] 任务通知模拟邮箱接收，值：%d\r\n", received_notify_val);
        
        /* 阶段 3: 控制执行节拍 */
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
