#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "sys.h"

// 引脚定义
// 行引脚 (中断输入)
#define KBD_ROW1_PIN    GPIO_PIN_0
#define KBD_ROW2_PIN    GPIO_PIN_1
#define KBD_ROW3_PIN    GPIO_PIN_2
#define KBD_ROW4_PIN    GPIO_PIN_10

// 列引脚 (扫描输出/读取)
#define KBD_COL1_PIN    GPIO_PIN_11
#define KBD_COL2_PIN    GPIO_PIN_12
#define KBD_COL3_PIN    GPIO_PIN_13
#define KBD_COL4_PIN    GPIO_PIN_14

#define KBD_PORT        GPIOB

// 函数声明
void keyboard_init(void);
uint8_t keyboard_get_value(void);

#endif
