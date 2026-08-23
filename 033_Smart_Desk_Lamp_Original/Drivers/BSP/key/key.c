#include "key.h"
#include "delay.h"

//初始化GPIO
void KeyInit(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    KEY1_GPIO_CLK_ENABLE();
    KEY2_GPIO_CLK_ENABLE();
    KEY3_GPIO_CLK_ENABLE();
    KEY4_GPIO_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=KEY1_GPIO_PIN;
    gpio_initstruct.Mode=GPIO_MODE_INPUT;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KEY1_GPIO_PORT,&gpio_initstruct);
    
    gpio_initstruct.Pin=KEY2_GPIO_PIN;
    HAL_GPIO_Init(KEY2_GPIO_PORT,&gpio_initstruct);
    
    gpio_initstruct.Pin=KEY3_GPIO_PIN;
    HAL_GPIO_Init(KEY3_GPIO_PORT,&gpio_initstruct);
    
    gpio_initstruct.Pin=KEY4_GPIO_PIN;
    HAL_GPIO_Init(KEY4_GPIO_PORT,&gpio_initstruct);
}

//函数:按键状态扫描
uint8_t KeyScan(void)
{
    static uint8_t key_up = 1;
    uint8_t key_val = 0;
    
    if(key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
    {
        delay_ms(10);
        key_up = 0;
        
        if(KEY1 == 0)
        {
            key_val = 1;
        }
        
        if(KEY2 == 0)
        {
            key_val = 2;
        }
        
        if(KEY3 == 0)
        {
            key_val = 3;
        }
        
        if(KEY4 == 0)
        {
            key_val = 4;
        }
    }
    else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
    {
        key_up = 1;
    }
    
    return key_val;
}
