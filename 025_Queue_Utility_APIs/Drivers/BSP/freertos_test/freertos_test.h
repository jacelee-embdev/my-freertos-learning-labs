#ifndef __FREERTOS_TEST_H__
#define __FREERTOS_TEST_H__

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

/**
 * @brief  初始化系统测试任务
 * @note   按照原逻辑创建 queue 及 task1, task2, task3
 */
void app_freertos_test_init(void);

#endif /* __FREERTOS_TEST_H__ */
