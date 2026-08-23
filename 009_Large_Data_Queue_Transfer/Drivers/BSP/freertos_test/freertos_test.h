#ifndef __FREERTOS_TEST_H__
#define __FREERTOS_TEST_H__

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 定义测试消息传输类型：指向字符缓冲区的指针
typedef char* TeriSink;

/**
 * @brief  初始化系统测试任务
 * @details 负责创建消息队列及相关的并发任务（task1, task2, task3）
 */
void app_freertos_test_init(void);

#endif /* __FREERTOS_TEST_H__ */
