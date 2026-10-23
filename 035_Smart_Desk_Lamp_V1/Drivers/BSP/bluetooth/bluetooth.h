#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "sys.h"

#define UART2_RX_BUF_SIZE   128
#define UART2_TX_BUF_SIZE   128

#define UART_EOK        0
#define UART_EARROR     1
#define UART_ETIMEOUT   2
#define UART_EINVAL     3

void bluetooth_init(uint32_t baudrate);

void uart2_rx_clar(void);
void bluetooh_send(char * format,...);
uint8_t bt_rx_get(void);

#endif
