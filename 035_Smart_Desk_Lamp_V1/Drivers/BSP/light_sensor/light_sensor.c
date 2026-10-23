#include "light_sensor.h"
#include "sys.h"

// 定义 ADC 句柄，用于管理 ADC1 的状态
ADC_HandleTypeDef adc_handle = {0};

/**
 * @brief  ADC 初始化函数
 * @note   配置 ADC 工作模式为：单次转换、非扫描、软件触发
 */
void adc_init(void)
{
    adc_handle.Instance = ADC1;                                // 指定使用 ADC1
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;           // 数据右对齐（12位数据存储在低12位，常用）
    adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;           // 关闭扫描模式（只测一个通道）
    adc_handle.Init.ContinuousConvMode = DISABLE;              // 关闭连续转换（单次模式：测一次就停，省电）
    adc_handle.Init.NbrOfConversion = 1;                       // 转换通道数量：1个
    adc_handle.Init.DiscontinuousConvMode = DISABLE;           // 关闭间断模式
    adc_handle.Init.NbrOfDiscConversion = 0;                   // 间断模式通道数（不用管）
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;     // 触发方式：软件触发（调用函数才开始测）
    
    // 应用上述配置
    HAL_ADC_Init(&adc_handle);
    
    // 【关键】执行 ADC 自校准
    // 上电初始化后必须校准一次，消除芯片制造误差，提高精度
    HAL_ADCEx_Calibration_Start(&adc_handle);
}

/**
 * @brief  ADC 底层硬件初始化回调函数
 * @note   此函数会被 HAL_ADC_Init 自动调用，负责开启时钟和配置 GPIO
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
        GPIO_InitTypeDef gpio_init_struct = {0};
        
        // 1. 开启外设时钟
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        // 2. 配置 GPIO 引脚 (PA1)
        gpio_init_struct.Pin = GPIO_PIN_1;
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;
        HAL_GPIO_Init(GPIOA,&gpio_init_struct);
        
        // 3. 配置 ADC 时钟分频
        adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;
        HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);
    }
}

/**
 * @brief  配置 ADC 通道参数
 * @param  channel: 要配置的通道 (如 ADC_CHANNEL_1)
 * @param  rank: 转换顺序 (通常为 RANK_1)
 * @param  samplingtime: 采样时间
 */
void adc_channel_config(ADC_HandleTypeDef* hadc,uint32_t channel,uint32_t rank,uint32_t samplingtime)
{
    ADC_ChannelConfTypeDef adc_ch_config = {0};
    
    adc_ch_config.Channel = channel;
    adc_ch_config.Rank = rank;
    adc_ch_config.SamplingTime = samplingtime;
    
    // 调用库函数应用配置
    HAL_ADC_ConfigChannel(hadc,&adc_ch_config);
}

/**
 * @brief  获取指定通道的转换结果
 * @param  channel: 要读取的通道 (如 ADC_CHANNEL_1)
 * @retval 12位的转换结果 (0~4095)
 */
uint32_t adc_get_result(uint32_t channel)
{
    // 1. 配置当前要读取的通道
    // 使用 239.5 周期采样时间，速度慢但精度高，适合读电位器等
    adc_channel_config(&adc_handle,channel,ADC_REGULAR_RANK_1,ADC_SAMPLETIME_239CYCLES_5);
    
    // 2. 软件触发开始转换
    HAL_ADC_Start(&adc_handle);
    
    // 3. 等待转换完成 (轮询模式)
    // 超时时间设为 10ms
    HAL_ADC_PollForConversion(&adc_handle,10);
    
    // 4. 读取数据寄存器并返回结果
    return (uint16_t)HAL_ADC_GetValue(&adc_handle);
}

void light_sensor_init(void)
{
    adc_init();
}

uint8_t light_sensor_get_value(void)
{
    return (uint8_t)(100 - adc_get_result(ADC_CHANNEL_1) *100 /4096);
}
