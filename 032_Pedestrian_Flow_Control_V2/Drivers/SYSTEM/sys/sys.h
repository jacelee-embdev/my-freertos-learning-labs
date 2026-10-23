#ifndef __SYS_H__
#define __SYS_H__

#include "stm32f1xx.h"

void SystemClock_Config(uint32_t plln);
void Error_Handler(void);

#endif
