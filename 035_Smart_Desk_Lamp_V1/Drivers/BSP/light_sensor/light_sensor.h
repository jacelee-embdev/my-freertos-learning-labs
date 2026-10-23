#ifndef __LIGHT_SENSOR_H__
#define __LIGHT_SENSOR_H__

#include "stdint.h"

void adc_init(void);
uint32_t adc_get_result(uint32_t channel);
void light_sensor_init(void);
uint8_t light_sensor_get_value(void);

#endif
