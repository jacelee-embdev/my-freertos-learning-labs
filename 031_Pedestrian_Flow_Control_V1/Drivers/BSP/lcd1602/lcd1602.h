#ifndef __LCD1602_H__
#define __LCD1602_H__

//RS引脚定义：寄存器选择引脚（0=指令寄存器，1=数据寄存器）
#define RS_GPIO_Port    GPIOB
#define RS_GPIO_Pin     GPIO_PIN_1
#define RS_HIGH     HAL_GPIO_WritePin(RS_GPIO_Port,RS_GPIO_Pin,GPIO_PIN_SET);
#define RS_LOW      HAL_GPIO_WritePin(RS_GPIO_Port,RS_GPIO_Pin,GPIO_PIN_RESET);

//RW引脚定义：读写选择引脚（0=写操作，1=读操作） 
#define RW_GPIO_Port    GPIOB
#define RW_GPIO_Pin     GPIO_PIN_2
#define RW_HIGH     HAL_GPIO_WritePin(RW_GPIO_Port,RW_GPIO_Pin,GPIO_PIN_SET);
#define RW_LOW      HAL_GPIO_WritePin(RW_GPIO_Port,RW_GPIO_Pin,GPIO_PIN_RESET);

//EN引脚定义：使能引脚（高电平有效，下降沿锁存数据）
#define EN_GPIO_Port    GPIOB
#define EN_GPIO_Pin     GPIO_PIN_10
#define EN_HIGH     HAL_GPIO_WritePin(EN_GPIO_Port,EN_GPIO_Pin,GPIO_PIN_SET);
#define EN_LOW      HAL_GPIO_WritePin(EN_GPIO_Port,EN_GPIO_Pin,GPIO_PIN_RESET);


void lcd1602_init(void);
void lcd1602_gpio_init(void);
void lcd1602_start(void);

void lcd1602_write_cmd(char cmd);
void lcd1602_write_data(char data_show);

void lcd1602_show_char(char row,char col,char data);
void lcd1602_show_char_string(char row,char col,char *string);
    
#endif
