#include "exti.h"
#include "delay.h"

uint8_t infrared_sensor_flag = FALSE;

//函数:中断初始化
void exti_init(void )
{
    GPIO_InitTypeDef gpio_initstruct;
    
    //打开时钟
    EXTI_PORT_CLK_ENABLE();
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin=EXTI_PIN;
    gpio_initstruct.Mode=GPIO_MODE_IT_FALLING;
    gpio_initstruct.Pull=GPIO_PULLUP;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EXTI_PORT,&gpio_initstruct);
      
    // 配置EXTI0中断的优先级（中断通道：EXTI0_IRQn，对应PA0的外部中断）
    HAL_NVIC_SetPriority(EXTI4_IRQn,2,0);
    
    //使能EXTI0中断通道（打开中断的"开关"）
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

//中断服务函数(ISR)
void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(EXTI_PIN);
}

//中断回调函数（Callback）
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(30);
    if(GPIO_Pin==EXTI_PIN)
    {
        if(HAL_GPIO_ReadPin(EXTI_PORT,EXTI_PIN)==GPIO_PIN_RESET)
            infrared_sensor_flag = TRUE;
    }
}

//函数:把"标志位"的值传出去
uint8_t get_infrared_sensor_flag(void)
{
    uint8_t temp=infrared_sensor_flag;
    infrared_sensor_flag=FALSE;
    return temp;
}

//函数:在它处修改"标志位的值"
void set_infrared_sensor_flag(uint8_t value)
{
    infrared_sensor_flag=value;
}

//函数：获取红外传感器当前电平状态
uint8_t get_infrared_sensor_status(void)
{
    // PB4为低电平表示有人，高电平表示人
    return HAL_GPIO_ReadPin(EXTI_PORT, EXTI_PIN);
}
