#include "hcsr04.h"
#include "sys.h"
#include "delay.h"
#include "uart1.h"

// TIM2定时器句柄，用于超声波计时
TIM_HandleTypeDef tim2_handle = {0};

// TIM2初始化：72分频→1MHz计数（1us/次），最大计时65535us
void tim2_init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    tim2_handle.Instance = TIM2;
    tim2_handle.Init.Prescaler = 72-1;    // 预分频系数
    tim2_handle.Init.Period = 65536-1;    // 自动重装值
    tim2_handle.Init.CounterMode = TIM_COUNTERMODE_UP;  // 向上计数
    tim2_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&tim2_handle);
}

// TIM2底层初始化：使能时钟（HAL库回调）
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
//{
//    if(htim->Instance == TIM2)
//    {
//        __HAL_RCC_TIM2_CLK_ENABLE();  // 开启TIM2时钟
//    }
//}

// 启动TIM2计数
void tim2_start(void)
{
    HAL_TIM_Base_Start(&tim2_handle);
}

// 停止TIM2计数
void tim2_stop(void)
{
    HAL_TIM_Base_Stop(&tim2_handle);
}

// 获取TIM2当前计数值
uint16_t get_tim2_cnt(void)
{
    return __HAL_TIM_GetCounter(&tim2_handle);
}

// 设置TIM2计数值
void set_tim2_cnt(uint16_t val)
{
    __HAL_TIM_SetCounter(&tim2_handle,val);
}

// HC-SR04 GPIO初始化：TRIG输出，ECHO下拉输入
void hcsr04_gpio_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    
    // 开启GPIO时钟
    TRIG_GPIO_CLK_ENABLE();
    ECHO_GPIO_CLK_ENABLE();
    
    // 配置TRIG引脚：推挽输出
    gpio_initstruct.Pin=TRIG_PIN;
    gpio_initstruct.Mode=GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull=GPIO_NOPULL;
    gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TRIG_PORT,&gpio_initstruct);
    
    // 配置ECHO引脚：下拉输入
    gpio_initstruct.Pin=ECHO_PIN;
    gpio_initstruct.Mode=GPIO_MODE_INPUT;
    gpio_initstruct.Pull=GPIO_PULLDOWN;
    HAL_GPIO_Init(ECHO_PORT,&gpio_initstruct);
}

// HC-SR04初始化：整合定时器和GPIO
void hcsr04_init(void)
{
    tim2_init();
    hcsr04_gpio_init();
}

// 获取HC-SR04测量距离（单位：cm）
float get_length_from_hcsr04(void)
{
    uint16_t total_time = 0;
    float distance = 0.0;
    
    // 发送15us高电平触发超声波
    TRIG_HIGH();
    delay_us(15);
    TRIG_LOW();
    
    // 等待ECHO拉高，开始计时
    while(ECHO_STATUS() == GPIO_PIN_RESET);
    set_tim2_cnt(0);
    tim2_start();
    
    // 等待ECHO拉低，停止计时
    while(ECHO_STATUS() == GPIO_PIN_SET);
    tim2_stop();

    // 读取计时时间（us）
    total_time = get_tim2_cnt();
    
    // 计算距离：时间*声速/2（0.0175为简化系数）
    distance = total_time * 0.0175 ;
    return distance;
}
