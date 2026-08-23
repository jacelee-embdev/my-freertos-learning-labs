#include "key.h"
#include "delay.h"

/* ---------------------------------------------------------------------------------- */
/* [底层驱动层] 负责按键外设引脚的硬件级初始化                                        */
/* ---------------------------------------------------------------------------------- */

// 初始化按键 GPIO 配置（内部上拉输入）
void key_init(void)
{
    GPIO_InitTypeDef gpio_config;
    
    /* 阶段 1: 开启外设时钟总线 */
    KEY_PORT_CLK_ENABLE();
    
    /* 阶段 2: 配置引脚参数 */
    // 按下时引脚接地(低电平)，因此空闲状态必须使用内部上拉(PULLUP)以维持高电平，防止悬空误触发
    gpio_config.Pin   = KEY1_PIN | KEY2_PIN;
    gpio_config.Mode  = GPIO_MODE_INPUT;
    gpio_config.Pull  = GPIO_PULLUP;
    gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(KEY_PORT, &gpio_config);
}

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层] 负责按键时序扫描、消抖与业务状态解析                                  */
/* ---------------------------------------------------------------------------------- */

// 轮询扫描并返回当前激活的键值
uint8_t key_get_active(void)
{
    uint16_t wait_timeout = 0;  // 局部句柄：用于记录 while 循环阻塞的超时时间
    
    /* 阶段 1: 轮询检测 KEY1 */
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY1_PIN) == GPIO_PIN_RESET)
    {
        // 滤除按键触点闭合瞬间的机械物理抖动毛刺
        delay_ms(88);
        
        // 再次采样，若依然为低电平，判定为有效按下动作
        if (HAL_GPIO_ReadPin(KEY_PORT, KEY1_PIN) == GPIO_PIN_RESET)
        {
            // 阻塞式等待松开，加入超时防护防止按键物理损坏导致 CPU 死锁
            wait_timeout = 0;
            while (HAL_GPIO_ReadPin(KEY_PORT, KEY1_PIN) == GPIO_PIN_RESET)
            {
                delay_ms(1);
                wait_timeout++;
                if (wait_timeout >= KEY_TIMEOUT_MS)
                {
                    break;  // 达到超时阈值，强制跳出
                }
            }
            
            return KEY_STATE_PRESSED_1;
        }
    }
    
    /* 阶段 2: 轮询检测 KEY2 */
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY2_PIN) == GPIO_PIN_RESET)
    {
        // 滤除机械抖动
        delay_ms(80);
        
        // 再次采样确认
        if (HAL_GPIO_ReadPin(KEY_PORT, KEY2_PIN) == GPIO_PIN_RESET)
        {
            // 阻塞式等待松开，加入超时防护
            wait_timeout = 0;
            while (HAL_GPIO_ReadPin(KEY_PORT, KEY2_PIN) == GPIO_PIN_RESET)
            {
                delay_ms(1);
                wait_timeout++;
                if (wait_timeout >= KEY_TIMEOUT_MS)
                {
                    break;  // 达到超时阈值，强制跳出
                }
            }
            
            return KEY_STATE_PRESSED_2;
        }
    }
    
    /* 阶段 3: 默认分支，无有效动作 */
    return KEY_STATE_NONE;
}
