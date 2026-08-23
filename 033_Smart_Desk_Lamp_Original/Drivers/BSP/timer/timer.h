#ifndef __TIMER_H__
#define __TIMER_H__

#include "stdint.h"

void timer_init(uint16_t arr,uint16_t psc);
void timer_start(void);
void timer_stop(void);
void timer_toggle(void);
uint32_t get_sit_time(void);
void set_sit_time(uint32_t val);

#endif
