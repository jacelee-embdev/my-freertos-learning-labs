#include "sys.h"
#include "delay.h"
#include "uart1.h"
#include "led.h"
#include "FreeRTOS.h"
#include "freertos_test.h"
#include "task.h"

int main(void)
{
    HAL_Init(); /* 初始化HAL库 */
    SystemClock_Config(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init();
    led_init(); /* 初始化LED灯 */
    uart1_init(115200);
    printf("hello world!\r\n");
    freertos_test();
    vTaskStartScheduler();
}
