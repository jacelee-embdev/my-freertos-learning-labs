#ifndef __BEEP_H__
#define __BEEP_H__

#define BEEP_PORT                GPIOB
#define BEEP_PIN                 GPIO_PIN_13
#define BEEP_PORT_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

void beep_init(void);
void beep_on(void);
void beep_off(void);

#endif
