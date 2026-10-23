**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 任务辅助 API 实验一

## 项目简介

本实验集中演示任务优先级、任务句柄和任务挂起相关的 FreeRTOS API。task1 首先读取 task2 的初始优先级，再把它从 1 调整为 5；随后使用任务名取得 task2 句柄并将其挂起。最后，task1 通过 `xTaskGetCurrentTaskHandle()` 获得自身句柄并挂起自己。串口日志按执行顺序显示每一步结果，便于理解句柄既可以在创建时保存，也可以在运行时查询。

## 主要功能

- 使用 `uxTaskPriorityGet()` 读取任务优先级。
- 使用 `vTaskPrioritySet()` 动态修改优先级。
- 通过 `xTaskGetHandle()` 按任务名查找句柄。
- 获取当前任务句柄并演示任务自挂起。

## 硬件连接

本实验主要通过串口观察。USART1 使用 PA9（TX）、PA10（RX），波特率为 115200。工程也初始化了 PA0、PA1 按键，但 task2 很快会被挂起，因此核心流程无需按键操作。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。复位后按顺序查看优先级读取、修改、按名称获取句柄以及两个任务被挂起的日志。

## 主要文件

- `Drivers/BSP/freertos_test/`：任务辅助 API 测试流程。
- `Users/FreeRTOSConfig.h`：可选任务 API 配置。
- `Drivers/SYSTEM/uart1/`：测试结果输出。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
