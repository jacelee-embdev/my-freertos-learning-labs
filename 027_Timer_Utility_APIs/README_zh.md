**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 软件定时器辅助 API 实验

## 项目简介

本实验在单次和周期软件定时器的基础上，演示定时器周期、ID 和到期时间查询。timer1 是 5 秒单次定时器，ID 为 1；timer2 是 1 秒自动重载定时器，ID 为 2。KEY1、KEY2 分别控制两者启停。监控任务每秒读取 timer1 的周期和 ID，并用 timer2 的到期节拍减去当前系统节拍，显示距离下一次到期的剩余节拍。

## 主要功能

- 查询软件定时器的周期和用户 ID。
- 读取下次到期节拍并计算剩余时间。
- 查询定时器是否活动并由按键切换状态。
- 使用公共回调统计两个定时器的运行次数。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。按 KEY1 启动 5 秒单次定时器，按 KEY2 启动 1 秒周期定时器。定时器处于活动状态时，可观察周期、ID、剩余节拍和回调次数。

## 主要文件

- `Drivers/BSP/freertos_test/`：定时器查询、监控与回调。
- `Drivers/BSP/key/`：定时器控制输入。
- `Users/FreeRTOSConfig.h`：定时器服务配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
