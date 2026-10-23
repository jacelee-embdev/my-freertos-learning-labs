#ifndef __RELAY_GATE_H__
#define __RELAY_GATE_H__

#include "sys.h"

#define RELAY_GATE_PORT                GPIOB
#define RELAY_GATE_PIN                 GPIO_PIN_6
#define RELAY_GATE_PORT_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()

void relay_gate_init(void);
void relay_gate_on(void);
void relay_gate_off(void);

#endif
