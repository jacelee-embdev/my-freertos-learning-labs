#include "sys.h"
#include "timer.h"
#include "led.h"

// 定时器句柄结构体：HAL库用于管理定时器的配置、状态等信息，初始化为0（默认状态）
TIM_HandleTypeDef timer_handle = {0};

uint32_t sit_time = 0;
uint8_t timer_runnning = 0;

/**
  * @brief  定时器TIM4初始化
  * @param  arr: 自动重装值
  * @param  psc: 预分频系数
  * @retval 无
  */
void timer_init(uint16_t arr,uint16_t psc)
{
    timer_handle.Instance = TIM4;                  // 选择TIM4
    timer_handle.Init.Prescaler = psc;             // 设置预分频系数
    timer_handle.Init.Period = arr;                // 设置自动重装值
    timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;  // 向上计数模式
    timer_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // 禁用自动重装预装载

    HAL_TIM_Base_Init(&timer_handle);              // 初始化定时器
    HAL_TIM_Base_Start_IT(&timer_handle);          // 启动定时器并开启中断
}

/**
  * @brief  定时器底层硬件初始化（由HAL库自动调用）
  * @param  htim: 定时器句柄指针
  * @retval 无
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4)
    {
         __HAL_RCC_TIM4_CLK_ENABLE();               // 使能TIM4时钟
        HAL_NVIC_SetPriority(TIM4_IRQn, 2, 2);     // 配置TIM4中断优先级（抢占2，响应2）
        HAL_NVIC_EnableIRQ(TIM4_IRQn);             // 使能TIM4中断;
    }
}

/**
  * @brief  TIM4中断服务函数（中断入口）
  * @retval 无
  */
void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&timer_handle);      // HAL库中断统一处理
}

/**
  * @brief  定时器更新中断回调函数
  * @param  htim: 定时器句柄指针
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4)
    {
        sit_time++;
//        Led1Toggle();
    }
}


void timer_start(void)
{
    HAL_TIM_Base_Start_IT(&timer_handle);
    timer_runnning = 1;
}

void timer_stop(void)
{
    HAL_TIM_Base_Stop_IT(&timer_handle);
    timer_runnning = 0;
}

void timer_toggle(void)
{
    if(timer_runnning == 1)
    {
        timer_stop();
    }
    else
    {
        timer_start();
    }
}

uint32_t get_sit_time(void)
{
    return sit_time;
}

void set_sit_time(uint32_t val)
{
    sit_time = val;
}
