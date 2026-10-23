#include "led.h"

/* ---------------------------------------------------------------------------------- */
/* [底层初始化层] 负责外设时钟使能、GPIO 模式配置与初始状态确立                       */
/* ---------------------------------------------------------------------------------- */

// 初始化 LED1 与 LED2 的 GPIO 及时钟，确立默认安全状态
void led_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;

    /* 阶段 1: 使能目标 GPIO 端口的外设时钟 */
    LED1_CLK_ENABLE();
    LED2_CLK_ENABLE();

    /* 阶段 2: 配置引脚工作模式为推挽输出，适用于直接驱动 LED 硬件 */
    gpio_initstruct.Mode  = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull  = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;

    // 挂载并初始化 LED1 引脚参数
    gpio_initstruct.Pin   = LED1_PIN;
    HAL_GPIO_Init(LED1_PORT, &gpio_initstruct);

    // 挂载并初始化 LED2 引脚参数
    gpio_initstruct.Pin   = LED2_PIN;
    HAL_GPIO_Init(LED2_PORT, &gpio_initstruct);

    /* 阶段 3: 确立初始安全状态（默认输出高电平熄灭，防止上电瞬间闪烁） */
    led1_off();
    led2_off();
}

/* ---------------------------------------------------------------------------------- */
/* [应用层: LED1 控制接口] 封装 PB8 的电平状态控制 API                                */
/* ---------------------------------------------------------------------------------- */

// 点亮 LED1 (PB8)
void led1_on(void)
{
    // 驱动引脚输出低电平，导通 LED 电流回路（假设硬件为低电平触发点亮）
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET);
}

// 熄灭 LED1 (PB8)
void led1_off(void)
{
    // 释放引脚输出高电平，切断 LED 电流回路
    HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET);
}

// 翻转 LED1 (PB8) 状态
void led1_toggle(void)
{
    // 借助 HAL 库底层寄存器异或操作实现电平状态切换
    HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN);
}

/* ---------------------------------------------------------------------------------- */
/* [应用层: LED2 控制接口] 封装 PB9 的电平状态控制 API                                */
/* ---------------------------------------------------------------------------------- */

// 点亮 LED2 (PB9)
void led2_on(void)
{
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_RESET);
}

// 熄灭 LED2 (PB9)
void led2_off(void)
{
    HAL_GPIO_WritePin(LED2_PORT, LED2_PIN, GPIO_PIN_SET);
}

// 翻转 LED2 (PB9) 状态
void led2_toggle(void)
{
    HAL_GPIO_TogglePin(LED2_PORT, LED2_PIN);
}
