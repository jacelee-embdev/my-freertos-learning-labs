#include "keyboard.h"
#include "delay.h"

static volatile uint8_t g_key_value = 0;

// 键值映射表 (4x4矩阵)
static const uint8_t KEY_MAP[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// 配置为空闲检测模式：行配置为中断输入(上拉)，列输出低电平
static void Keyboard_Config_Idle(void)
{
    GPIO_InitTypeDef gpio_initstruct;

    gpio_initstruct.Pin = KBD_ROW1_PIN | KBD_ROW2_PIN | KBD_ROW3_PIN | KBD_ROW4_PIN;
    gpio_initstruct.Mode = GPIO_MODE_IT_FALLING; 
    gpio_initstruct.Pull = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KBD_PORT, &gpio_initstruct);
    
    gpio_initstruct.Pin = KBD_COL1_PIN | KBD_COL2_PIN | KBD_COL3_PIN | KBD_COL4_PIN;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(KBD_PORT, &gpio_initstruct);
    
    HAL_GPIO_WritePin(KBD_PORT, KBD_COL1_PIN | KBD_COL2_PIN | KBD_COL3_PIN | KBD_COL4_PIN, GPIO_PIN_RESET);
}

// 配置为扫描读取模式：行配置为推挽输出，列配置为输入(上拉)
static void Keyboard_Config_Scan(void)
{
    GPIO_InitTypeDef gpio_initstruct;

    gpio_initstruct.Pin = KBD_ROW1_PIN | KBD_ROW2_PIN | KBD_ROW3_PIN | KBD_ROW4_PIN;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(KBD_PORT, &gpio_initstruct);
    
    gpio_initstruct.Pin = KBD_COL1_PIN | KBD_COL2_PIN | KBD_COL3_PIN | KBD_COL4_PIN;
    gpio_initstruct.Mode = GPIO_MODE_INPUT;
    gpio_initstruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KBD_PORT, &gpio_initstruct);
}

void keyboard_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 初始化为中断等待模式
    Keyboard_Config_Idle();
    
    // 配置行引脚对应的中断优先级
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    
    HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
    
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// 获取并清除当前键值
uint8_t keyboard_get_value(void)
{
    uint8_t ch = 0;
    if(g_key_value != 0)
    {
        ch = g_key_value;
        g_key_value = 0;
    }
    return ch;
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KBD_ROW1_PIN);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KBD_ROW2_PIN);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KBD_ROW3_PIN);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KBD_ROW4_PIN);
}

// 核心中断回调：执行消抖、行列反转扫描
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    int8_t row_idx = -1;
    int8_t col_idx = -1;
    
    // 软件消抖 (10ms)
    delay_ms(10); 
    
    // 确认按键是否依然按下
    if(HAL_GPIO_ReadPin(KBD_PORT, GPIO_Pin) != GPIO_PIN_RESET) {
        return;
    }

    // 锁定触发中断的行索引
    if(GPIO_Pin == KBD_ROW1_PIN) row_idx = 0;
    else if(GPIO_Pin == KBD_ROW2_PIN) row_idx = 1;
    else if(GPIO_Pin == KBD_ROW3_PIN) row_idx = 2;
    else if(GPIO_Pin == KBD_ROW4_PIN) row_idx = 3;
    
    if(row_idx == -1) return;

    // 切换IO方向进入扫描模式
    Keyboard_Config_Scan(); 
    
    // 仅拉低当前触发的行，其他行拉高
    HAL_GPIO_WritePin(KBD_PORT, KBD_ROW1_PIN|KBD_ROW2_PIN|KBD_ROW3_PIN|KBD_ROW4_PIN, GPIO_PIN_SET);
    
    uint16_t target_row_pin = 0;
    switch(row_idx) {
        case 0: target_row_pin = KBD_ROW1_PIN; break;
        case 1: target_row_pin = KBD_ROW2_PIN; break;
        case 2: target_row_pin = KBD_ROW3_PIN; break;
        case 3: target_row_pin = KBD_ROW4_PIN; break;
    }
    HAL_GPIO_WritePin(KBD_PORT, target_row_pin, GPIO_PIN_RESET);
    
    // 读取列引脚，低电平即为按下的列
    if(HAL_GPIO_ReadPin(KBD_PORT, KBD_COL1_PIN) == GPIO_PIN_RESET) col_idx = 0;
    else if(HAL_GPIO_ReadPin(KBD_PORT, KBD_COL2_PIN) == GPIO_PIN_RESET) col_idx = 1;
    else if(HAL_GPIO_ReadPin(KBD_PORT, KBD_COL3_PIN) == GPIO_PIN_RESET) col_idx = 2;
    else if(HAL_GPIO_ReadPin(KBD_PORT, KBD_COL4_PIN) == GPIO_PIN_RESET) col_idx = 3;
    
    // 查表保存键值
    if(row_idx >= 0 && col_idx >= 0)
    {
        g_key_value = KEY_MAP[row_idx][col_idx];
    }

    // 恢复为中断监听模式并清除标志位
    Keyboard_Config_Idle();
    __HAL_GPIO_EXTI_CLEAR_IT(KBD_ROW1_PIN | KBD_ROW2_PIN | KBD_ROW3_PIN | KBD_ROW4_PIN);
}
