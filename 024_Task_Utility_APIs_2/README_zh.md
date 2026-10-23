**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 任务辅助 API 实验二

## 项目简介

本实验演示如何查询 FreeRTOS 系统中的任务数量、状态和栈使用情况。task1 在启动后调用 `vTaskList()` 输出任务列表，再根据 `uxTaskGetNumberOfTasks()` 动态申请 `TaskStatus_t` 数组，并通过 `uxTaskGetSystemState()` 获取所有任务的状态数据。随后程序按名称找到 task1，使用 `vTaskGetInfo()` 读取其优先级、运行计数和栈高水位线，最后释放临时数组。

## 主要功能

- 输出任务名称、状态、优先级、栈余量和编号。
- 动态申请数组以保存系统任务状态快照。
- 获取指定任务的详细信息和栈高水位线。
- 使用完状态数组后及时释放 FreeRTOS 堆内存。

## 硬件连接

USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200，是本实验的主要观察接口。KEY1、KEY2 位于 PA0、PA1，可用于确认第二个任务仍在扫描输入。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。复位后查看任务列表、系统状态表及 task1 的详细信息；之后 task1 每 6 秒输出一次运行状态。

## 主要文件

- `Drivers/BSP/freertos_test/`：任务列表和状态查询逻辑。
- `Users/FreeRTOSConfig.h`：跟踪及格式化功能配置。
- `Drivers/SYSTEM/uart1/`：状态表输出。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
