#ifndef __UART1_H__
#define __UART1_H__

#include "sys.h"
#include "stdio.h"

/**
 * @brief  [DEBUG] 临界段保护格式化打印宏
 * @param  ...: 可变参数列表，用法同标准 printf
 * @note   1. 解决多任务环境下 printf 资源抢占导致的串口乱码或数据断裂问题。
 * 2. 采用 __VA_ARGS__ 方式定义，解决了 Keil 静态分析提示参数不匹配的红线警告。
 * 3. 临界段会屏蔽系统调度与中断，仅限调试使用，严禁在高频任务或大数据量场景下调用。
 */
#define debug_printf(...)  do {     \
    taskENTER_CRITICAL();           \
    printf(__VA_ARGS__);            \
    taskEXIT_CRITICAL();            \
} while(0)

/* ================================================================ */
/* GPIO 硬件配置宏定义                  */
/* ================================================================ */

/* 发送引脚 (TX) 配置 */
#define UART1_TX_PORT              GPIOA                           /* TX 端口 */
#define UART1_TX_PIN               GPIO_PIN_9                      /* TX 引脚 */
#define UART1_TX_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()    /* TX 端口时钟 */

/* 接收引脚 (RX) 配置 */
#define UART1_RX_PORT              GPIOA                           /* RX 端口 */
#define UART1_RX_PIN               GPIO_PIN_10                     /* RX 引脚 */
#define UART1_RX_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()    /* RX 端口时钟 */

/* ================================================================ */

/* 接收缓冲区大小 */
#define UART1_RX_BUF_SIZE          128


/* 函数声明 */
void uart1_init(uint32_t baudrate);
void uart1_rx_clear(void);

#endif
