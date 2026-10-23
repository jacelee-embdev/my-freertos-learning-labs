#include "sys.h"
#include "led.h"
#include "oled.h"

uint8_t led_level = 0;
TIM_HandleTypeDef pwm_handle = {0}; 

/**
  * @brief  PWM初始化函数
  * @param  arr: 自动重装值(TIMx_ARR寄存器值)，决定PWM周期
  * @param  psc: 预分频系数(TIMx_PSC寄存器值)，用于分频定时器时钟
  * @note   周期计算公式：T = (arr + 1) * (psc + 1) / TIMx_CLK
  *         TIM3挂载在APB1总线上，默认时钟为72MHz(STM32F1系列)
  * @retval 无
  */
void pwm_init(uint16_t arr,uint16_t psc) 
{
    // 定义PWM输出比较初始化结构体，初始化为0
    TIM_OC_InitTypeDef pwm_config = {0}; 
    
    // 配置定时器4核心参数
    pwm_handle.Instance = TIM3; 
    pwm_handle.Init.Prescaler = psc; 
    pwm_handle.Init.Period = arr; 
    pwm_handle.Init.CounterMode = TIM_COUNTERMODE_UP; 
    HAL_TIM_PWM_Init(&pwm_handle);      // 初始化定时器PWM模式
    
    // 配置PWM通道3
    pwm_config.OCMode = TIM_OCMODE_PWM1; 
    pwm_config.Pulse = 0; 
    pwm_config.OCPolarity = TIM_OCPOLARITY_HIGH; 
    HAL_TIM_PWM_ConfigChannel(&pwm_handle,&pwm_config,TIM_CHANNEL_3); 
    
    // 启动PWM输出
    HAL_TIM_PWM_Start(&pwm_handle,TIM_CHANNEL_3); 
}

// PWM底层初始化函数
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) 
{
    // 仅处理TIM3的初始化
    if(htim->Instance == TIM3) 
    {
            GPIO_InitTypeDef gpio_initstruct; 
        // 使能TIM3时钟（APB1总线外设）
        __HAL_RCC_TIM3_CLK_ENABLE(); 
        // 使能GPIOB时钟（PB8属于GPIOB）
        __HAL_RCC_GPIOB_CLK_ENABLE(); 
        
        // 配置PB8引脚为TIM3_CH3复用功能
        gpio_initstruct.Pin=GPIO_PIN_0; 
        gpio_initstruct.Mode=GPIO_MODE_AF_PP;   // 模式：复用推挽输出
        gpio_initstruct.Pull=GPIO_PULLUP; 
        gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH; 
        HAL_GPIO_Init(GPIOB,&gpio_initstruct); 
    }
}

/**
  * @brief  设置PWM占空比（修改比较值CCR3）
  * @param  val: 比较值（范围：0 ~ 自动重装值arr）
  * @note   val越大，低电平有效时间越长（因极性为低电平有效）
  * @retval 无
  */
void set_pwm_compare(uint16_t val) 
{
    // 直接修改TIM3通道3的比较值（CCR3寄存器）
    __HAL_TIM_SET_COMPARE(&pwm_handle,TIM_CHANNEL_3,val); 
}

void led_init(void)
{
    pwm_init(500-1,72-1);
}

void led_off(void)
{
    set_pwm_compare(0);
    led_level = 0;
    oled_show_Chinese(70,2,17);
}

void led_low(void)
{
    set_pwm_compare(150);
    led_level = 1;
    oled_show_Chinese(70,2,14);
}

void led_medium(void)
{
    set_pwm_compare(300);
    led_level = 2;
    oled_show_Chinese(70,2,15);
}


void led_high(void)
{
    set_pwm_compare(450);
    led_level = 3;
    oled_show_Chinese(70,2,16);
    
}

uint8_t led_level_get(void)
{
    return led_level;
}

