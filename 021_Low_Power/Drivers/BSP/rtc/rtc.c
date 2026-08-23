#include "rtc.h"
#include "led.h"
#include "uart1.h"

/* ---------------------------------------------------------------------------------- */
/* [私有资源与句柄] 模块内部使用的外设句柄 (严格物理隔离)                             */
/* ---------------------------------------------------------------------------------- */

static RTC_HandleTypeDef hrtc = {0};                                                    // RTC 硬件句柄，禁止外部直接访问

/* ---------------------------------------------------------------------------------- */
/* [底层驱动层] HAL 库回调与硬件中断服务                                              */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief RTC 底层硬件初始化 (时钟源、EXTI 线及中断优先级配置)
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc_ptr)
{
    RCC_OscInitTypeDef       osc_initstruct       = {0};
    RCC_PeriphCLKInitTypeDef periphclk_initstruct = {0};
    
    /* 1. 权限开启：开启电源与备份域访问，这是操作 RTC 寄存器的前提 */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BKP_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    
    /* 2. 时钟源配置：强制使用 LSE (32.768kHz) 以确保低功耗下的计时精度 */
    __HAL_RCC_RTC_ENABLE();
    
    osc_initstruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    osc_initstruct.LSEState       = RCC_LSE_ON;
    osc_initstruct.PLL.PLLState   = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&osc_initstruct);

    periphclk_initstruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    periphclk_initstruct.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&periphclk_initstruct);
    
    /* 3. 唤醒链路配置：配置第 17 线 EXTI (RTC 闹钟信号专用) */
    __HAL_RTC_ALARM_EXTI_ENABLE_IT(); 
    __HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();

    /* 4. 中断向量配置：设置闹钟中断优先级并使能 */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 5, 0); 
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**
 * @brief RTC 闹钟中断服务函数入口
 */
void RTC_Alarm_IRQHandler(void)
{
    /* 翻转硬件心跳灯，直观反馈 Tickless 唤醒频率 */
    led2_toggle(); 
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

/**
 * @brief RTC 闹钟事件回调 (由 HAL 库调用)
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc_ptr)
{
    printf("闹钟响了\r\n");
}

/* ---------------------------------------------------------------------------------- */
/* [逻辑处理层] 基于寄存器级优化的唤醒机制                                            */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief 关闭 RTC 唤醒功能
 */
void bsp_rtc_stop_wakeup(void)
{
    __HAL_RTC_ALARM_DISABLE_IT(&hrtc, RTC_IT_ALRA);
}

/**
 * @brief 设定目标唤醒时间并启动闹钟
 * @param time_ms 目标休眠时长 (毫秒)
 */
void bsp_rtc_set_wakeup(uint32_t time_ms)
{
    uint32_t wake_ticks = time_ms;                                                      // 预设分频已实现 1ms/Tick
    if (wake_ticks == 0) wake_ticks = 1;

    /* 1. 互斥同步：等待上一次寄存器写入操作完成，防止总线死锁 */
    while((hrtc.Instance->CRL & RTC_CRL_RTOFF) == 0);

    /* 2. 状态刷新：强制同步 RTC 时间至 APB1 总线，确保读取的计数值实时准确 */
    hrtc.Instance->CRL &= ~RTC_CRL_RSF;
    while((hrtc.Instance->CRL & RTC_CRL_RSF) == 0);

    /* 3. 增量计算：获取当前计数值并累加目标偏移量 */
    uint32_t current_ticks = (hrtc.Instance->CNTH << 16) | hrtc.Instance->CNTL;
    uint32_t target_ticks  = current_ticks + wake_ticks;

    /* 4. 配置原子性：进入配置模式写入闹钟寄存器 (ALRH/ALRL) */
    while((hrtc.Instance->CRL & RTC_CRL_RTOFF) == 0);
    hrtc.Instance->CRL |= RTC_CRL_CNF;
    hrtc.Instance->ALRH = target_ticks >> 16;
    hrtc.Instance->ALRL = target_ticks & 0xFFFF;
    hrtc.Instance->CRL &= ~RTC_CRL_CNF;

    /* 5. 写入确认：退出配置模式并等待硬件同步完成，确保进入 STOP 前数据已入库 */
    while((hrtc.Instance->CRL & RTC_CRL_RTOFF) == 0);

    /* 6. 使能中断：激活硬件闹钟中断信号 */
    __HAL_RTC_ALARM_ENABLE_IT(&hrtc, RTC_IT_ALRA);
}

/* ---------------------------------------------------------------------------------- */
/* [应用接口层] 外设初始化                                                            */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief RTC 初始化配置，设置预分频器以匹配 1ms 计时周期
 */
void rtc_init(void)
{
    hrtc.Instance          = RTC;
    hrtc.Init.AsynchPrediv = 32;                                                        // LSE (32768Hz) / 33 ≈ 1ms 计时粒度
    hrtc.Init.OutPut       = RTC_OUTPUTSOURCE_NONE;

    HAL_RTC_Init(&hrtc);          
}
