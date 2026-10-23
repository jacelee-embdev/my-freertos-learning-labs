#ifndef __EXTI_H__
#define __EXTI_H__

#include "sys.h"


#define TRUE 1
#define FALSE 0

#define EXTI_PORT                GPIOB
#define EXTI_PIN                 GPIO_PIN_4
#define EXTI_PORT_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

void exti_init(void );
uint8_t get_infrared_sensor_flag(void);
void set_infrared_sensor_flag(uint8_t value);
uint8_t get_infrared_sensor_status(void);

#endif
