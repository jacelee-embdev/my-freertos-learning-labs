// app_sys_orchestrator.h
#ifndef APP_SYS_ORCHESTRATOR_H__
#define APP_SYS_ORCHESTRATOR_H__

/* ---------------------------------------------------------------------------------- */
/* [应用接口区] 对外公开的 API 与任务创建入口                                         */
/* ---------------------------------------------------------------------------------- */

// 系统的统筹入口：负责分配所有子任务与初始化 OS 级通信资源
void app_sys_orchestrator_init(void);

#endif /* APP_SYS_ORCHESTRATOR_H__ */
