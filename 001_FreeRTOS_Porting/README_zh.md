**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 移植实验

## 项目简介

本实验将 FreeRTOS 移植到 STM32F103C8T6，并通过两个独立 LED 任务验证内核能否正常启动和调度。系统时钟配置为 72 MHz，FreeRTOS 节拍频率为 1 kHz。调度器启动后，LED1 以 1 秒为间隔亮灭，LED2 以 500 毫秒为间隔亮灭，用不同频率的并发现象直观验证任务切换与阻塞延时。

## 主要功能

- 完成 FreeRTOS 内核、移植层和 `FreeRTOSConfig.h` 的基础配置。
- 使用 `xTaskCreate()` 创建两个动态任务，优先级分别为 2 和 3。
- 使用 `vTaskDelay()` 让任务阻塞并释放 CPU。
- 通过 USART1 输出启动调试信息。

## 硬件连接

LED1 连接 PB8，LED2 连接 PB9，均为低电平点亮。USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200，可连接串口工具查看调试输出。

## 使用方法

使用 Keil MDK 打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译并下载到开发板。复位后观察两个 LED 以不同周期持续闪烁。

## 主要文件

- `Users/FreeRTOSConfig.h`：FreeRTOS 内核配置。
- `Drivers/BSP/freertos_test/`：测试任务的创建与运行逻辑。
- `Users/main.c`：硬件初始化和调度器启动入口。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
