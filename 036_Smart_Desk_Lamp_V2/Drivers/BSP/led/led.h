#ifndef __LED_H__
#define __LED_H__

#include "stdint.h"

// PWM初始化函数
void pwm_init(uint16_t arr,uint16_t psc);

// 设置PWM占空比函数
void set_pwm_compare(uint16_t val);
void led_init(void);
void led_off(void);
void led_low(void);
void led_medium(void);
void led_high(void);
uint8_t led_level_get(void);


#endif
