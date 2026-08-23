#include "freertos_test.h"
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "rtc.h"
#include <stdio.h> 

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄]                                                                   */
/* ---------------------------------------------------------------------------------- */

static TaskHandle_t led_task_handle = NULL;                                             // LED 低功耗任务句柄

/* ---------------------------------------------------------------------------------- */
/* [物理层] 系统级回调与硬件时序恢复                                                  */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief 进入 Tickless 深度睡眠前的前置配置
 * @param ulExpectedIdleTime 预期的空闲时间 (单位视 OS 配置而定)
 */
void PreSleepProcessing(uint32_t ulExpectedIdleTime) 
{
    /* 1. 硬件唤醒源配置：停止当前闹钟并根据预期空闲时间重设 RTC 唤醒周期 */
    bsp_rtc_stop_wakeup();
    bsp_rtc_set_wakeup(ulExpectedIdleTime);
    
    /* 2. 关闭不必要的 GPIO 时钟以降低静态功耗 */
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    
    /* 3. 系统指令：清除唤醒标志并置位深度睡眠控制位 (SLEEPDEEP) */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; 
}

/**
 * @brief 从 Tickless 模式唤醒后的时钟与外设恢复
 * @param ulExpectedIdleTime 预期的空闲时间
 */
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
    /* 1. 硬件状态恢复：立即清除深度睡眠控制位 */
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

    /* 2. 时钟树恢复：从 STOP 模式唤醒后系统默认切换至内核 HSI，需重新锁相至最高频率 */
    SystemClock_Config(RCC_PLL_MUL9); 
    
    /* 3. 恢复外设时钟供电 */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层] 业务逻辑实现                                                          */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief LED 自动翻转任务 (配合 Tickless 模式自动进入低功耗)
 */
static void led_lowpower_task(void *pvParameters)
{
    uint8_t led_state = 0;                                                              // 本地状态变量：0-灭，1-亮
    
    while (1)
    {
        /* 根据当前状态执行物理操作 */
        if (led_state == 0) 
        {
            led1_on();
            debug_printf("[System] 唤醒: 执行开灯操作 \r\n");
            led_state = 1;
        } 
        else
        {
            led1_off();
            debug_printf("[System] 唤醒: 执行关灯操作 \r\n");
            led_state = 0;
        }

        /* 进入 OS 阻塞延时 (5000ms)，触发内核 Tickless 机制 */
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}

/* ---------------------------------------------------------------------------------- */
/* [应用层] 外部接口                                                                  */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief 初始化测试模块并创建低功耗演示任务
 */
void app_freertos_test_init(void)
{
    xTaskCreate(led_lowpower_task, 
                "LED_LP", 
                128, 
                NULL, 
                2, 
                &led_task_handle);

    debug_printf("系统初始化完成. LED 循环已启动, 即将进入 Tickless 模式.\r\n");
}
