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

void led1_on(void);
void led1_off(void);
void led1_toggle(void);

void led2_on(void);
void led2_off(void);
void led2_toggle(void);

#endif /* __LED_H__ */
