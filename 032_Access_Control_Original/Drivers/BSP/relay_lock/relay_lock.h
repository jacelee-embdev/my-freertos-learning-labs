#ifndef __RELAY_LOCK_H__
#define __RELAY_LOCK_H__

#include "stdint.h"

#define RELAY_GATE_ON  0
#define RELAY_GATE_OFF 1

#define RELAY_LOCK_PIN                 GPIO_PIN_6
#define RELAY_LOCK_PORT                GPIOB
#define RELAY_LOCK_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

void relay_lock_init(void);
void relay_lock_on(void);
void relay_lock_off(void);

#endif
