#include "uart1.h"
#include "string.h"

/* 串口句柄 */
UART_HandleTypeDef uart1_handle;

/* 接收缓冲区 */
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
uint16_t uart1_rx_len = 0;

/**
 * @brief       重定义fputc (供printf使用)
 */
int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);       /* 等待发送完成 */
    USART1->DR = (uint8_t)ch;               /* 发送数据 */
    return ch;
}

/**
 * @brief       串口初始化
 */
void uart1_init(uint32_t baudrate)
{
    uart1_handle.Instance = USART1;                        /* 直接指定 USART1 */
    uart1_handle.Init.BaudRate = baudrate;
    uart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    uart1_handle.Init.StopBits = UART_STOPBITS_1;
    uart1_handle.Init.Parity = UART_PARITY_NONE;
    uart1_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart1_handle.Init.Mode = UART_MODE_TX_RX;
    
    HAL_UART_Init(&uart1_handle);
}

/**
 * @brief       HAL库底层初始化回调
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_struct;

    //判断是否是串口1
    if (huart->Instance == USART1)
    {
        /* 1. 使能时钟 */
        UART1_TX_CLK_ENABLE();                 /* 使能TX GPIO时钟 (宏定义) */
        UART1_RX_CLK_ENABLE();                 /* 使能RX GPIO时钟 (宏定义) */
        __HAL_RCC_USART1_CLK_ENABLE();          /* 使能USART1时钟 */

        /* 2. 配置TX引脚 */
        gpio_init_struct.Pin = UART1_TX_PIN;   /* 使用宏定义 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(UART1_TX_PORT, &gpio_init_struct);
                
        /* 3. 配置RX引脚 */
        gpio_init_struct.Pin = UART1_RX_PIN;   /* 使用宏定义 */
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(UART1_RX_PORT, &gpio_init_struct);
        
        /* 4. 配置中断 */
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);

        /* 5. 开启接收和空闲中断 */
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
        __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
    }
    
    //判断是否是串口2,防止影响其它串口初始化
    else if(huart->Instance == USART2)
    {
        // 1.开启时钟
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // 2.配置GPIO引脚
        GPIO_InitTypeDef gpio_initstruct;
        
        // --- 配置 TX (发送引脚 PA9)
        gpio_initstruct.Pin=GPIO_PIN_2;
        gpio_initstruct.Mode=GPIO_MODE_AF_PP;           // 复用推挽输出
        gpio_initstruct.Pull=GPIO_PULLUP;               // 上拉
        gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;     // 高速
        HAL_GPIO_Init(GPIOA,&gpio_initstruct);
        
        // --- 配置 RX (接收引脚 PA10)
        gpio_initstruct.Pin=GPIO_PIN_3;
        gpio_initstruct.Mode=GPIO_MODE_AF_INPUT;        //复用输入模式
        gpio_initstruct.Pull=GPIO_PULLUP;
        gpio_initstruct.Speed=GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA,&gpio_initstruct);
        
        // 3.配置 NVIC (中断控制器)
        HAL_NVIC_EnableIRQ(USART2_IRQn);            // 使能 USART1 全局中断通道
        HAL_NVIC_SetPriority(USART2_IRQn,3,3);      // 设置中断优先级
        
        // 4.使能串口中断原
        // 使能 RXEN (接收寄存器非空) 中断,当收到数据时触发中断
        __HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
        
        // 开启 IDLE (空闲) 中断：用于判断一包数据结束
        __HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);
    }
}

/**
 * @brief       清空缓冲区
 */
void uart1_rx_clear(void)
{
    memset(uart1_rx_buf, 0, sizeof(uart1_rx_buf));
    uart1_rx_len = 0;
}

/**
 * @brief       串口中断服务函数
 */
void USART1_IRQHandler(void)
{
    uint8_t receive_data = 0;   
    
    /* 接收数据中断 */
    if(__HAL_UART_GET_FLAG(&uart1_handle, UART_FLAG_RXNE) != RESET){
        if(uart1_rx_len >= sizeof(uart1_rx_buf))
            uart1_rx_len = 0;
            
        HAL_UART_Receive(&uart1_handle, &receive_data, 1, 1000);
        uart1_rx_buf[uart1_rx_len++] = receive_data;
    }

    /* 空闲中断 (一帧数据接收完毕) */
    if (__HAL_UART_GET_FLAG(&uart1_handle, UART_FLAG_IDLE) != RESET)
    {
        printf("recv: %s\r\n", uart1_rx_buf);
        uart1_rx_clear();
        __HAL_UART_CLEAR_IDLEFLAG(&uart1_handle);
    }
}
