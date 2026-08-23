#include "sys.h"
#include "delay.h"
#include "uart1.h"
#include "led.h"
#include "FreeRTOS.h"
#include "freertos_test.h"
#include "task.h"
#include "rtc.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init();
    led_init();
    uart1_init(115200);
    rtc_init();
    printf("Hello Wrld!\r\n");
    
    app_freertos_test_init();
    vTaskStartScheduler();
}
