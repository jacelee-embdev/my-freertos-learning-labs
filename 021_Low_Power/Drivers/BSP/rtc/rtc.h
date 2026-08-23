#ifndef __RTC_H__
#define __RTC_H__

#include "sys.h" 

/* ---------------------------------------------------------------------------------- */
/* [功能接口声明] 对外公开的 API 与功能函数                                          */
/* ---------------------------------------------------------------------------------- */

// 初始化 RTC 外设配置
void rtc_init(void);                                                                    

// 设置 RTC 唤醒目标时间 (单位: ms)
void bsp_rtc_set_wakeup(uint32_t time_ms);                                              

// 立即关闭 RTC 闹钟唤醒中断
void bsp_rtc_stop_wakeup(void);                                                         

#endif /* __RTC_H__ */