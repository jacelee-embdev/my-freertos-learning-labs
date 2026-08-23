#include "exti.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t led1_task_handle;

/* ---------------------------------------------------------------------------------- */
/* [物理层] 硬件初始化与低级配置                                                        */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief  初始化 EXTI Line1 (PA1) 外部中断
 * @note   配置为下降沿触发，上拉模式，优先级 [2,0]
 */
void exti_init(void)
{
    GPIO_InitTypeDef exti_cfg;
    
    // 1. 开启对应 GPIO 端口外设时钟
    EXTI_1_CLK_ENABLE();
    
    // 2. 配置引脚中断属性
    exti_cfg.Pin   = EXTI_1_PIN;
    exti_cfg.Mode  = GPIO_MODE_IT_FALLING;
    exti_cfg.Pull  = GPIO_PULLUP;
    exti_cfg.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EXTI_1_PORT, &exti_cfg);
    
    // 3. 配置 NVIC 中断优先级
    HAL_NVIC_SetPriority(EXTI_1_IRQn, 6, 0);
    
    // 4. 使能中断通道
    HAL_NVIC_EnableIRQ(EXTI_1_IRQn);
}

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层] 中断服务程序与回调处理                                                  */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief  EXTI0 中断服务入口
 */
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(EXTI_1_PIN);
}

/**
 * @brief  外部中断通用回调函数
 * @param  GPIO_Pin: 触发中断的引脚编号
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t ret;
    // 阶段 1: 软件消抖
    delay_ms(20);
//    vTaskDelay(20);       //不能在这里调用，会触发断言ASSERT
    
    // 阶段 2: 判定触发源并读取物理电平确认
    if (GPIO_Pin == EXTI_1_PIN)
    {
        if (HAL_GPIO_ReadPin(EXTI_1_PORT, EXTI_1_PIN) == GPIO_PIN_RESET)
        {
            ret = xTaskResumeFromISR( led1_task_handle);
            if(ret == pdTRUE)
            {
                printf("在中断中恢复 led1_task\r\n");
                /* ret为pdTRUE表示：新恢复的任务优先级高于当前被打断的任务 */
                /* 触发上下文切换(Context Switch)，确保中断退出后立即执行高优先级任务，实现零延迟响应 */
                portYIELD_FROM_ISR(ret);
            }
        }
    }
}
