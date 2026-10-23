#include "ia_sensor.h"
#include "delay.h"
#include "led.h"

uint8_t ia_sensor = FALSE;

//函数:中断初始化
void ia_sensor_init(void )
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=GPIO_PIN_12;
    gpio_initstruct.Mode=GPIO_MODE_INPUT;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_initstruct);
}

uint8_t get_ia_sensor_flag(void)
{
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12) == GPIO_PIN_RESET)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

