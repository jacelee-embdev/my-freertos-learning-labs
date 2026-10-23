#include "bluetooth.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"


// --- 全局变量定义 ---
uint8_t command  = 0;
uint8_t  uart2_rx_buf[UART2_RX_BUF_SIZE];   // 接收缓冲区，用于存放串口收到的数据
uint16_t uart2_rx_len = 0;                  // 当前接收到的数据长度

// UART句柄：HAL库核心结构体，保存串口配置
UART_HandleTypeDef uart2_handle = {0};

/**
 * @brief  初始化串口1
 * @param  baudrate: 波特率
 */
void bluetooth_init(uint32_t baudrate)
{
    // 1. 配置串口句柄的基本参数
    uart2_handle.Instance = USART2;                         //选择外设基地址:USART2
    uart2_handle.Init.BaudRate = baudrate;                  //设置波特率
    uart2_handle.Init.WordLength = UART_WORDLENGTH_8B;      //子长:8位数据
    uart2_handle.Init.StopBits = UART_STOPBITS_1;           //停止位:1位
    uart2_handle.Init.Parity = UART_PARITY_NONE;            //检验位:无
    uart2_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;      //硬件流控制:无
    uart2_handle.Init.Mode = UART_MODE_TX_RX;               //模式:收发模式
    
    // 2. 调用 HAL 库初始化函数
    HAL_UART_Init(&uart2_handle);
}


/**
 * @brief  串口1 中断服务函数
 * @note   处理数据接收和空闲检测
 */
void USART2_IRQHandler(void)
{
    uint8_t receive_data = 0;
    
    // --- 1. 处理 RXNE 中断 (收到数据) ---
    if(__HAL_UART_GET_FLAG(&uart2_handle, UART_FLAG_RXNE) != RESET)
    {
        // 读取一个字节 (读取操作会自动清除 RXNE 标志)
        HAL_UART_Receive(&uart2_handle, &receive_data, 1, 1000);
        
        // 将数据存入缓冲区 (含溢出保护)
        if(uart2_rx_len < sizeof(uart2_rx_buf))
        {
            uart2_rx_buf[uart2_rx_len++] = receive_data;
        }
        else
        {
            // 缓冲区满，覆盖最早的数据
            uart2_rx_len = 0;
            uart2_rx_buf[uart2_rx_len++] = receive_data;
        }
    }
    
    // --- 2. 处理 IDLE 中断 (数据包结束) ---
    if(__HAL_UART_GET_FLAG(&uart2_handle, UART_FLAG_IDLE) != RESET)
    {
        printf("bt recv:%s\r\n",uart2_rx_buf);         // 打印接收到的数据
        command = uart2_rx_buf[0];
        uart2_rx_clar();                            // 清空缓冲区，为下次接收做准备
        __HAL_UART_CLEAR_IDLEFLAG(&uart2_handle);   // 清除 IDLE 标志位 (防止重复进入中断)
    }
}

/**
 * @brief  清除接收缓冲区
 */
void uart2_rx_clar(void)
{
    memset(uart2_rx_buf, 0, sizeof(uart2_rx_buf));  // 清空缓冲区内存
    uart2_rx_len = 0;                               // 重置数据长度
}

void bluetooh_send(char * format,...)
{
    uint8_t send_buf[128] = {0};
    va_list arg;
    va_start(arg,format);
    vsprintf((char *)send_buf,format,arg);
    va_end(arg);
    
    HAL_UART_Transmit(&uart2_handle,send_buf,sizeof(send_buf),100);
}

uint8_t bt_rx_get(void)
{
    uint8_t temp = command;
    command = ' ';
    return temp;
}
