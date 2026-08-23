#include "sys.h"

/**
  * @brief  系统时钟配置
  * @note   配置 HSE, PLL, AHB, APB 从而将系统主频设置为 72MHz
  */
void SystemClock_Config(uint32_t plln)
{
    HAL_StatusTypeDef hal_restult = HAL_ERROR;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 1. 初始化晶振 (HSE) 和 PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = plln; // 8MHz * 9 = 72MHz
    hal_restult = HAL_RCC_OscConfig(&RCC_OscInitStruct);

    if (hal_restult != HAL_OK)
    {
        Error_Handler();
    }

    /* 2. 初始化 CPU, AHB 和 APB 总线时钟 */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // HCLK  = 72MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // PCLK1 = 36MHz (最大值)
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // PCLK2 = 72MHz
    hal_restult = HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_2);
    
    if (hal_restult != HAL_OK)
    {
        Error_Handler();
    }
}

/**
  * @brief  通用错误处理函数
  */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* 发生致命错误，程序在此死循环 */
    }
}
