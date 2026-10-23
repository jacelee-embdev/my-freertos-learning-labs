#include "beep.h"
#include "sys.h"

//初始化GPIO口
void beep_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    BEEP_PORT_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=BEEP_PIN;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BEEP_PORT,&gpio_initstruct);
    
    //关闭蜂鸣器
    beep_off();
}
    
//函数:打开蜂鸣器
void beep_on(void)
{
    HAL_GPIO_WritePin(BEEP_PORT,BEEP_PIN,GPIO_PIN_RESET);
}

//函数:关闭蜂鸣器
void beep_off(void)
{
    HAL_GPIO_WritePin(BEEP_PORT,BEEP_PIN,GPIO_PIN_SET);
}

