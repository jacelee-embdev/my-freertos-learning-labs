#include "relay_gate.h"


//初始化GPIO口
void relay_gate_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    RELAY_GATE_PORT_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=RELAY_GATE_PIN;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RELAY_GATE_PORT,&gpio_initstruct);
    
    //关闭relay_gate
    relay_gate_off();
}
    
//函数:点亮relay_gate
void relay_gate_on(void)
{
    HAL_GPIO_WritePin(RELAY_GATE_PORT,RELAY_GATE_PIN,GPIO_PIN_RESET);
}

//函数:熄灭relay_gate
void relay_gate_off(void)
{
    HAL_GPIO_WritePin(RELAY_GATE_PORT,RELAY_GATE_PIN,GPIO_PIN_SET);
}

//获取继电器当前状态
uint8_t relay_gate_status_get()
{
    return (uint8_t)HAL_GPIO_ReadPin(RELAY_GATE_PORT,RELAY_GATE_PIN);
}
