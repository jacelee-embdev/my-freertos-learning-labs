**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 互斥信号量实验

## 项目简介

本实验在与优先级翻转实验相似的三个任务场景中，改用 FreeRTOS 互斥量保护共享资源。低优先级任务持有互斥量时，如果高优先级任务请求该资源，内核会暂时提升持有者的优先级，使低优先级任务尽快完成并释放互斥量，减少中优先级任务造成的额外阻塞。资源释放后，任务优先级恢复原值，这就是互斥量的优先级继承机制。

## 主要功能

- 使用 `xSemaphoreCreateMutex()` 创建互斥量。
- 构造低、中、高三个优先级任务的资源竞争。
- 演示高优先级任务阻塞时触发优先级继承。
- 通过串口日志与 `012` 二值信号量实验进行对比。

## 硬件连接

本实验无需按键交互，主要使用 USART1 观察调度过程。USART1 使用 PA9（TX）、PA10（RX），波特率为 115200，并与串口模块共地。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。复位后观察 Low、Middle、High 日志，重点关注高优先级任务等待后，低优先级持有者获得执行机会并快速释放互斥量的过程。

## 主要文件

- `Drivers/BSP/freertos_test/`：互斥量与优先级继承场景。
- `Drivers/SYSTEM/delay/`：模拟任务负载。
- `Drivers/SYSTEM/uart1/`：任务执行日志。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
