**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 任务挂起与任务内恢复实验

## 项目简介

本实验演示如何在普通任务上下文中挂起和恢复另一个 FreeRTOS 任务。程序创建 LED1、LED2 和按键监控三个同优先级任务。LED1 每 1 秒改变一次亮灭状态，LED2 每 500 毫秒改变一次；按下 KEY1 后，监控任务通过 `vTaskSuspend()` 挂起 LED1 任务，按下 KEY2 后再通过 `vTaskResume()` 恢复它。LED2 始终运行，可用于观察调度器没有因单个任务挂起而停止。

## 主要功能

- 通过任务句柄控制指定任务的挂起与恢复。
- 对比被挂起任务和正常运行任务的 LED 现象。
- 在任务上下文中调用 `vTaskSuspend()` 与 `vTaskResume()`。
- 通过 USART1 输出任务状态变化。

## 硬件连接

LED1、LED2 分别连接 PB8、PB9，均为低电平点亮。KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉，按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

使用 Keil MDK 打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译并下载。按下 KEY1 后 LED1 停在当前状态；按下 KEY2 后 LED1 从挂起位置继续运行。

## 主要文件

- `Drivers/BSP/freertos_test/`：任务创建、挂起和恢复逻辑。
- `Drivers/BSP/key/`：双按键扫描与消抖。
- `Users/main.c`：硬件初始化与调度器启动。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
