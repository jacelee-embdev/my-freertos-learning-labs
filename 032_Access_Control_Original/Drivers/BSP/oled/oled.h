#ifndef __OLED_H__
#define __OLED_H__

#include "stdint.h"

enum message{
    SHOW_INPUT_PWD = 0,
    SHOW_PWD_RIGHT,
    SHOW_PWD_WRONG,
    SHOW_INPUT_OLD_PWD,
    SHOW_INPUT_NEW_PWD,
    SHOW_PWD_CHANGED,
    SHOW_SET_PWD
};

/* =========================================================================
 * 硬件引脚配置 (移植修改区)
 * 如果需要移植到其他引脚或STM32系列，请修改以下宏定义
 * =========================================================================
 */
// SCL (I2C时钟线) 引脚定义
#define OLED_IIC_SCL_CLK()      __HAL_RCC_GPIOB_CLK_ENABLE()    // 开启GPIOB时钟
#define OLED_IIC_SCL_PORT       GPIOB                              // SCL 端口
#define OLED_IIC_SCL_PIN        GPIO_PIN_8                        // SCL 引脚

// SDA (I2C数据线) 引脚定义
#define OLED_IIC_SDA_CLK()      __HAL_RCC_GPIOB_CLK_ENABLE()    // 开启GPIOB时钟
#define OLED_IIC_SDA_PORT       GPIOB                              // SDA 端口
#define OLED_IIC_SDA_PIN        GPIO_PIN_9                        // SDA 引脚


/* =========================================================================
 * 电平控制宏 (用于模拟I2C时序)
 * 通过直接操作GPIO寄存器或HAL库函数来翻转引脚电平
 * =========================================================================
 */
// SCL 信号线控制
#define OLED_SCL_RESET()        HAL_GPIO_WritePin(OLED_IIC_SCL_PORT,OLED_IIC_SCL_PIN,GPIO_PIN_RESET)    // 拉低 SCL
#define OLED_SCL_SET()          HAL_GPIO_WritePin(OLED_IIC_SCL_PORT,OLED_IIC_SCL_PIN,GPIO_PIN_SET)        // 拉高 SCL

// SDA 信号线控制
#define OLED_SDA_RESET()        HAL_GPIO_WritePin(OLED_IIC_SDA_PORT,OLED_IIC_SDA_PIN,GPIO_PIN_RESET)    // 拉低 SDA
#define OLED_SDA_SET()          HAL_GPIO_WritePin(OLED_IIC_SDA_PORT,OLED_IIC_SDA_PIN,GPIO_PIN_SET)        // 拉高 SDA


/* =========================================================================
 * 函数原型声明
 * =========================================================================
 */
/* --- 底层驱动接口 (内部调用) --- */
void oled_write_command(uint8_t cmd);        // 写命令
void oled_write_data(uint8_t data);            // 写数据

/* --- 应用层接口 (用户调用) --- */
void oled_init(void);                        // OLED初始化 (上电调用一次)
void oled_fill(uint8_t data);                // 全屏填充 (0x00:清屏, 0xFF:全亮)
void oled_set_cursor(uint8_t x,uint8_t y);    // 设置显示坐标 (x:0~127, y:0~7)

/* 字符与图形显示 */
// 显示单个ASCII字符 (size: 12/16/24)
void oled_show_char(uint8_t x,uint8_t y,uint8_t numb,uint8_t size);

// 显示字符串
void oled_show_string(uint8_t x,uint8_t y,char *string,uint8_t size);

// 显示汉字 (ch_string: 汉字数组索引)
void oled_show_Chinese(uint8_t x,uint8_t y,uint8_t ch_string,uint8_t size);

// 显示BMP图片 (width:像素宽, height:页高度)
void oled_show_imge(uint8_t x,uint8_t y,uint8_t width,uint8_t height,uint8_t *bmp);

void oled_show_input(void);
void oled_show_right(void);
void oled_show_wrong(void);
void oled_show_old(void);
void oled_show_new(void);
void oled_show_set(void);
void oled_show_changed(void);
#endif
