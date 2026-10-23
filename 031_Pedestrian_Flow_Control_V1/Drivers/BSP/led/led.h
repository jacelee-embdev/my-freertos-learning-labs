#ifndef __LED_H__
#define __LED_H__

#include "sys.h"

/* ---------------------------------------------------------------------------------- */
/* [硬件移植区] 集中放置引脚端口、时钟使能等底层硬件强相关宏定义                      */
/* ---------------------------------------------------------------------------------- */
#define LED1_PIN                 GPIO_PIN_8
#define LED1_PORT                GPIOB
#define LED1_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

#define LED2_PIN                 GPIO_PIN_9
#define LED2_PORT                GPIOB
#define LED2_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

/* ---------------------------------------------------------------------------------- */
/* [功能接口声明] 仅暴露对外公开的 API，提供给应用层调用                              */
/* ---------------------------------------------------------------------------------- */
void led_init(void);             // 初始化 LED1 与 LED2 的 GPIO 及时钟，确立默认安全状态

void led1_on(void);              // 点亮 LED1 (PB8)
void led1_off(void);             // 熄灭 LED1 (PB8)
void led1_toggle(void);          // 翻转 LED1 (PB8) 状态

void led2_on(void);              // 点亮 LED2 (PB9)
void led2_off(void);             // 熄灭 LED2 (PB9)
void led2_toggle(void);          // 翻转 LED2 (PB9) 状态

#endif /* __LED_H__ */
