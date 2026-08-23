#include "sys.h"
#include "delay.h"
#include "uart1.h"
#include "led.h"
#include "FreeRTOS.h"
#include "app_sys_orchestrator.h"
#include "task.h"
#include "beep.h"
#include "exti.h"
#include "lcd1602.h"
#include "relay_gate.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init();
    led_init();
    uart1_init(115200);
    beep_init();
    exti_init();
    lcd1602_init();
    relay_gate_init();
    printf("Hello Wrld!\r\n");
    
    
    app_sys_orchestrator_init();
    vTaskStartScheduler();
}
