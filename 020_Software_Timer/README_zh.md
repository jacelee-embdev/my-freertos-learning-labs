**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 软件定时器实验

## 项目简介

本实验演示 FreeRTOS 单次软件定时器和周期软件定时器的创建与控制。timer1 是周期为 5000 个系统节拍的单次定时器，在当前 1 kHz 节拍配置下约为 5 秒；timer2 是周期为 1000 个节拍、约 1 秒的自动重载定时器。KEY1、KEY2 分别切换两个定时器的启动和停止状态，公共回调函数通过定时器 ID 区分来源并打印触发次数。

## 主要功能

- 创建一个单次定时器和一个自动重载定时器。
- 使用定时器 ID 在公共回调中区分实例。
- 通过 `xTimerIsTimerActive()` 查询当前状态。
- 使用双按键动态启动或停止对应定时器。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。USART1 使用 PA9（TX）、PA10（RX），波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。KEY1 控制 5 秒单次定时器，KEY2 控制 1 秒周期定时器；终端会显示启停状态及每次回调的累计次数。

## 主要文件

- `Drivers/BSP/freertos_test/`：定时器创建、控制与回调。
- `Drivers/BSP/key/`：定时器启停输入。
- `Users/FreeRTOSConfig.h`：软件定时器服务任务配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
