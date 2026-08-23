#ifndef FREERTOS_TEST_H__
#define FREERTOS_TEST_H__

#include "sys.h" 

/* ---------------------------------------------------------------------------------- */
/* [系统钩子区] FreeRTOS Tickless 模式回调声明                                        */
/* ---------------------------------------------------------------------------------- */

void PreSleepProcessing(uint32_t ulExpectedIdleTime);                                   // 进入低功耗模式前置处理
void PostSleepProcessing(uint32_t ulExpectedIdleTime);                                  // 退出低功耗模式恢复处理

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

void app_freertos_test_init(void);                                                      // 模块初始化与任务创建入口

#endif /* FREERTOS_TEST_H__ */
