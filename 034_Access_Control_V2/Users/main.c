#include "sys.h"
#include "delay.h"
#include "uart1.h"
#include "FreeRTOS.h"
#include "app_sys_orchestrator.h"
#include "task.h"
#include "led.h"
#include "beep.h"
#include "keyboard.h"
#include "oled.h"
#include "password.h"
#include "relay_lock.h"
#include "w25q128.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init();
    led_init();
    uart1_init(115200);
    beep_init();
    keyboard_init();
    oled_init();
    password_init();
    relay_lock_init();
    w25q128_init();
    printf("Hello Wrld!\r\n");
    
//    w25q128_chip_erase(); //清除W25128所有内容,恢复初始状态
    
    app_sys_orchestrator_init();
    vTaskStartScheduler();

}
