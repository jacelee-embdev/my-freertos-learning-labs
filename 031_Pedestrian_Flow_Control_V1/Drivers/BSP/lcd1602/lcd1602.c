#include "delay.h"
#include "lcd1602.h"


/**
 * @brief  LCD1602初始化总入口函数
 * @note   包含GPIO初始化和LCD上电时序初始化
 * @param  无
 * @retval 无
 */
void lcd1602_init(void)
{
    //初始化GPIO口
    lcd1602_gpio_init();
    //上电初始化
    lcd1602_start(); 
}

/**
 * @brief  LCD1602相关GPIO口初始化函数
 * @note   配置GPIOA（数据口D0~D7）和GPIOB（RS/RW/EN）为推挽输出模式，上拉，高速
 * @param  无
 * @retval 无
 */
void lcd1602_gpio_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    //调用GPIO初始化函数,初始化A组所需GPIO口
    gpio_initstruct.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
                        GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&gpio_initstruct);
    
    //调用GPIO初始化函数,初始化B组所需GPIO口
    gpio_initstruct.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_initstruct);
}

/**
 * @brief  LCD1602上电初始化时序函数
 * @note   严格按照LCD1602手册的上电初始化流程执行，确保稳定启动
 * @param  无
 * @retval 无
 */
void lcd1602_start(void)
{
    //（1） 延时 15ms
    delay_ms(15);
    //（2） 写指令 38H(不检测忙信号)
    lcd1602_write_cmd(0x38);
    //（3） 延时 5ms
    delay_ms(5);
    //（4） 以后每次写指令，读/写数据操作均需要检测忙信号
    //（5） 写指令 38H：显示模式设置
    lcd1602_write_cmd(0x38);
    //（6） 写指令 08H：显示关闭
    lcd1602_write_cmd(0x08);
    //（7） 写指令 01H：显示清屏
    lcd1602_write_cmd(0x01);
    //（8） 写指令 06H：显示光标移动设置
    lcd1602_write_cmd(0x06);
    //（9） 写指令 0CH：显示开及光标设置
    lcd1602_write_cmd(0x0C);
}

/**
 * @brief  向LCD1602写入指令函数
 * @note   通过控制RS/RW/EN引脚，将指令码写入LCD指令寄存器
 * @param  cmd: 要写入的指令码（如显示模式、清屏、光标设置等）
 * @retval 无
 */
void lcd1602_write_cmd(char cmd)
{
    RS_LOW;
    RW_LOW;
    EN_LOW;
    GPIOA->ODR = cmd;
    delay_ms(5);
    EN_HIGH;
    delay_ms(5);
    EN_LOW;
}

/**
 * @brief  向LCD1602写入显示数据函数
 * @note   通过控制RS/RW/EN引脚，将字符数据写入LCD数据寄存器
 * @param  dataShow: 要显示的字符数据（ASCII码）
 * @retval 无
 */
void lcd1602_write_data(char dataShow)
{
    RS_HIGH;
    RW_LOW;
    EN_LOW;
    GPIOA->ODR = dataShow;
    delay_ms(5);
    EN_HIGH;
    delay_ms(5);
    EN_LOW;
}

/**
 * @brief LCD1602指定行列显示单个字符
 * @param row 显示的行号（LCD1602一般为1或2行，需结合硬件地址映射）
 * @param col 显示的列号（LCD1602一行最多16列，范围0~15）
 * @param data 要显示的字符数据（ASCII码）
 */
void lcd1602_show_char(char row,char col,char data)
{
    //在哪里显示?
    lcd1602_write_cmd(row+col);
    
    //显示什么?
    lcd1602_write_data(data);
}

/**
 * @brief  LCD1602指定位置显示字符串函数
 * @note   支持第一行和第二行显示，从指定列开始显示字符串
 * @param  row: 显示行号（1=第一行，2=第二行）
 * @param  col: 显示列偏移（从0开始，范围0~15）
 * @param  string: 要显示的字符串指针（以'\0'结尾）
 * @retval 无
 */
void lcd1602_show_char_string(char row,char col,char *string)
{
    switch(row)
    {
        case 1:
            lcd1602_write_cmd(0x80+col);         //设置第一行显示起始地址：0x80（第一行基地址）+列偏移col
            while(*string)
            {
                lcd1602_write_data(*string);
                string++;
            }
            break;
        
        case 2:
            lcd1602_write_cmd(0x80+0x40+col);       //设置第二行显示起始地址：0x80+0x40（第二行基地址）+列偏移col
            while(*string)
            {
                lcd1602_write_data(*string);
                string++;
            }
            break;
    }
}
