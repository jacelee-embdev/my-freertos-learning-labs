**简体中文** | [English](README.md)

# My FreeRTOS Learning Labs

## 仓库简介

这是一个面向 STM32F103C8T6 的 FreeRTOS 个人学习实验仓库，用于整理学习过程中的示例工程、验证代码和实践记录。仓库中的每个子工程对应一个相对独立的实验，内容从 FreeRTOS 移植与任务管理开始，逐步覆盖队列、信号量、互斥量、事件组、任务通知、软件定时器、低功耗、内存管理以及常用辅助 API，最后通过几个综合案例练习模块划分与代码重构。

本仓库首先是一份学习档案和代码备忘录，并非官方教程、通用组件库或可直接用于量产的工程。保留这些代码的主要目的，是方便日后快速查阅和复现实验，同时借助 Git 的版本历史与 GitHub 远程仓库降低本地文件意外丢失的风险。

## 仓库用途

- 集中保存 FreeRTOS 学习和调试过程中编写的实验代码。
- 记录不同知识点的最小实践，作为以后开发时的参考和备忘。
- 使用 Git 跟踪修改过程，便于比较、回退和恢复历史版本。
- 上传到 GitHub 保存远程副本，方便在不同设备间访问和同步。
- 通过中英文 README 说明每个实验的目标、原理和运行现象。

## 完整实验索引

单击工程名称可查看对应的中文说明。

| 编号 | 工程 | 内容 |
|---:|---|---|
| 000 | [FreeRTOS_LTS_Source_v202406_04](000_FreeRTOS_LTS_Source_v202406_04/README_zh.md) | 保存 FreeRTOS LTS 202406.04 源码快照，用于内核和配套组件源码查阅、移植参考。 |
| 001 | [FreeRTOS_Porting](001_FreeRTOS_Porting/README_zh.md) | 将 FreeRTOS 移植到 STM32F103C8T6，并以两个不同周期的 LED 任务验证调度。 |
| 002 | [Dynamic_Task_Creation](002_Dynamic_Task_Creation/README_zh.md) | 动态创建三个任务，并通过按键按任务句柄删除 LED1 任务。 |
| 003 | [Static_Task_Creation](003_Static_Task_Creation/README_zh.md) | 使用静态栈与 TCB 创建任务，并为内核空闲任务和定时器任务提供静态内存。 |
| 004 | [Task_Suspend_Resume_From_Task](004_Task_Suspend_Resume_From_Task/README_zh.md) | 在普通任务中通过按键挂起和恢复指定 LED 任务。 |
| 005 | [Task_Suspend_Resume_From_ISR](005_Task_Suspend_Resume_From_ISR/README_zh.md) | 在任务中挂起 LED 任务，再通过外部中断安全恢复并按需触发上下文切换。 |
| 006 | [Printf_Garbled_Output_Fix](006_Printf_Garbled_Output_Fix/README_zh.md) | 使用临界区保护多任务 `printf()`，避免串口消息交叉、断裂或乱码。 |
| 007 | [Relative_Vs_Absolute_Delay](007_Relative_Vs_Absolute_Delay/README_zh.md) | 对比相对延时与绝对延时，观察任务执行时间造成的周期漂移差异。 |
| 008 | [Message_Queue](008_Message_Queue/README_zh.md) | 使用字节队列在按键发送任务与阻塞接收任务之间传递小型消息。 |
| 009 | [Large_Data_Queue_Transfer](009_Large_Data_Queue_Transfer/README_zh.md) | 通过队列传递静态缓冲区指针，演示减少大块数据复制开销的方法。 |
| 010 | [Binary_Semaphore](010_Binary_Semaphore/README_zh.md) | 用两个按键演示二值信号量的释放、获取以及空满状态下的失败结果。 |
| 011 | [Counting_Semaphore](011_Counting_Semaphore/README_zh.md) | 用按键增减计数型信号量，观察资源计数在 0 到 5 之间变化。 |
| 012 | [Priority_Inversion](012_Priority_Inversion/README_zh.md) | 使用低、中、高优先级任务和普通二值信号量复现优先级翻转。 |
| 013 | [Mutex_Semaphore](013_Mutex_Semaphore/README_zh.md) | 将共享资源改用互斥量保护，演示优先级继承如何缓解优先级翻转。 |
| 014 | [Queue_Set](014_Queue_Set/README_zh.md) | 使用队列集让单个任务统一等待数据队列和二值信号量两类事件源。 |
| 015 | [Event_Group](015_Event_Group/README_zh.md) | 用事件组等待两个按键事件同时到达，并在条件满足后自动清除事件位。 |
| 016 | [Task_Notification_As_Binary_Semaphore](016_Task_Notification_As_Binary_Semaphore/README_zh.md) | 清零式获取任务通知，使多个待处理通知合并为一次二值事件。 |
| 017 | [Task_Notification_As_Counting_Semaphore](017_Task_Notification_As_Counting_Semaphore/README_zh.md) | 逐次递增和递减通知值，模拟可积压事件的计数型信号量。 |
| 018 | [Task_Notification_As_Mailbox](018_Task_Notification_As_Mailbox/README_zh.md) | 将 32 位任务通知用作覆盖式单槽邮箱，只保留最新发送值。 |
| 019 | [Task_Notification_As_Event_Group](019_Task_Notification_As_Event_Group/README_zh.md) | 通过 `eSetBits` 设置通知位，模拟单接收任务的轻量级事件组。 |
| 020 | [Software_Timer](020_Software_Timer/README_zh.md) | 创建单次和自动重载软件定时器，并用按键控制启停、统计回调次数。 |
| 021 | [Low_Power](021_Low_Power/README_zh.md) | 启用 Tickless Idle，在空闲期进入 STOP 模式并通过 RTC 闹钟唤醒。 |
| 022 | [Memory_Management](022_Memory_Management/README_zh.md) | 使用 `heap_4` 申请、释放内存，并周期查询 FreeRTOS 剩余堆空间。 |
| 023 | [Task_Utility_APIs_1](023_Task_Utility_APIs_1/README_zh.md) | 演示任务优先级读写、按名称获取句柄以及挂起指定任务和当前任务。 |
| 024 | [Task_Utility_APIs_2](024_Task_Utility_APIs_2/README_zh.md) | 查询任务数量、状态、运行计数与栈高水位线，并输出系统任务列表。 |
| 025 | [Queue_Utility_APIs](025_Queue_Utility_APIs/README_zh.md) | 查询队列剩余槽位和待处理消息数，并演示队列填满后的复位操作。 |
| 026 | [Semaphore_Utility_APIs](026_Semaphore_Utility_APIs/README_zh.md) | 查询互斥量当前持有者，并结合三优先级任务观察优先级继承。 |
| 027 | [Timer_Utility_APIs](027_Timer_Utility_APIs/README_zh.md) | 查询软件定时器的周期、ID、活动状态和下一次到期时间。 |
| 028 | [Event_Group_Utility_APIs_1](028_Event_Group_Utility_APIs_1/README_zh.md) | 非破坏性读取事件组状态，观察已置位标志在读取后继续保留。 |
| 029 | [Event_Group_Utility_APIs_2](029_Event_Group_Utility_APIs_2/README_zh.md) | 使用 `xEventGroupSync()` 构建三个逻辑参与者的多任务同步屏障。 |
| 030 | [Memory_Utility_APIs](030_Memory_Utility_APIs/README_zh.md) | 获取空闲空间、最大连续空闲块及累计分配和释放次数等堆统计信息。 |
| 031 | [Pedestrian_Flow_Control_V1](031_Pedestrian_Flow_Control_V1/README_zh.md) | 使用红外计数、闸机、LCD、LED 和蜂鸣器实现五人限流及 3 秒自动恢复。 |
| 032 | [Pedestrian_Flow_Control_V2](032_Pedestrian_Flow_Control_V2/README_zh.md) | 保留五人限流功能，并以状态机、执行器和独立显示任务重构应用层。 |
| 033 | [Access_Control_V1](033_Access_Control_V1/README_zh.md) | 实现带 W25Q128 密码存储、矩阵键盘输入、错误报警和自动落锁的门禁系统。 |
| 034 | [Access_Control_V2](034_Access_Control_V2/README_zh.md) | 保留完整门禁功能，并重构密码状态机、消息结构及软硬件职责边界。 |
| 035 | [Smart_Desk_Lamp_V1](035_Smart_Desk_Lamp_V1/README_zh.md) | 集成人体感应、自动调光、测距与久坐报警、OLED 显示和蓝牙控制。 |
| 036 | [Smart_Desk_Lamp_V2](036_Smart_Desk_Lamp_V2/README_zh.md) | 保留三种工作模式，并重构共享状态、阈值配置、初始化和任务协作。 |

## 开发环境

- MCU：STM32F103C8T6
- 实时操作系统：FreeRTOS
- 外设库：STM32 HAL
- 工程工具：Keil MDK-ARM

不同实验使用的外设、串口输出和硬件连接可能不同，请以对应子工程的 README 与源码配置为准。

## 快速开始

1. 克隆或下载本仓库。
2. 选择需要学习的实验目录。
3. 使用 Keil MDK-ARM 打开该目录下的 `Projects/MDK-ARM/STM32F103.uvprojx`。
4. 根据实际开发板检查时钟、引脚和下载器配置，然后编译并烧录。
5. 结合串口输出、LED 或其他外设现象观察实验结果。

## 许可证

本仓库使用 [MIT License](LICENSE)。第三方源码仍遵循其各自的许可证和声明。
