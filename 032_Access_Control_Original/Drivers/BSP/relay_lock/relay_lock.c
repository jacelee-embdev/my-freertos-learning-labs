#include "sys.h"
#include "relay_lock.h"


//初始化GPIO口
void relay_lock_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    RELAY_LOCK_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=RELAY_LOCK_PIN;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RELAY_LOCK_PORT,&gpio_initstruct);
    
    //关闭relay_lock
    relay_lock_off();
}
    
//函数:点亮relay_lock
void relay_lock_on(void)
{
    HAL_GPIO_WritePin(RELAY_LOCK_PORT,RELAY_LOCK_PIN,GPIO_PIN_RESET);
}

//函数:熄灭relay_lock
void relay_lock_off(void)
{
    HAL_GPIO_WritePin(RELAY_LOCK_PORT,RELAY_LOCK_PIN,GPIO_PIN_SET);
}

//获取继电器当前状态
uint8_t relay_lock_status_get()
{
    return (uint8_t)HAL_GPIO_ReadPin(RELAY_LOCK_PORT,RELAY_LOCK_PIN);
}
