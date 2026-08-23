#include "sys.h"
#include "delay.h"
#include "uart1.h"
#include "led.h"
#include "FreeRTOS.h"
#include "app_sys_orchestrator.h"
#include "task.h"
#include "beep.h"
#include "bluetooth.h"
#include "hcsr04.h"
#include "ia_sensor.h"
#include "key.h"
#include "light_sensor.h"
#include "oled.h"
#include "timer.h"


int main(void)
{
    HAL_Init();
    SystemClock_Config(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init();
    led_init();
    uart1_init(115200);
    beep_init();
    bluetooth_init(115200);
    hcsr04_init();
    ia_sensor_init();
    KeyInit();
    light_sensor_init();
    oled_init();
    timer_init(1000-1,7200-1);
    printf("Hello Wrld!\r\n");
    
    
    app_sys_orchestrator_init();
    vTaskStartScheduler();
}
