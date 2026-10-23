**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Tickless 低功耗实验

## 项目简介

本实验启用 FreeRTOS Tickless Idle，并在任务长时间阻塞时让 STM32F103 进入深度睡眠。LED 任务每 5 秒切换一次状态，其余时间系统通过睡眠前后处理钩子配置 STOP 模式。进入睡眠前，程序使用 32.768 kHz LSE 驱动 RTC 闹钟并关闭部分 GPIO 时钟；RTC 到期后产生唤醒中断，程序重新配置 72 MHz 系统时钟并恢复外设时钟。

## 主要功能

- 启用 `configUSE_TICKLESS_IDLE` 和睡眠处理钩子。
- 根据预期空闲节拍设置 RTC 闹钟唤醒时间。
- 睡眠前关闭 GPIOB、GPIOC、GPIOD 时钟。
- STOP 唤醒后恢复 PLL 时钟树和 LED 外设。

## 硬件连接

LED1、LED2 分别连接 PB8、PB9。USART1 使用 PA9/PA10。RTC 使用外部 32.768 kHz LSE 晶振；若开发板未安装或未起振，唤醒计时可能无法正常工作。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。复位后 LED1 约每 5 秒切换，LED2 在 RTC 闹钟中断时翻转。需要测量功耗时，可在电源回路串接电流表比较运行和休眠电流。

## 主要文件

- `Drivers/BSP/freertos_test/`：Tickless 睡眠前后处理。
- `Drivers/BSP/rtc/`：LSE、RTC 闹钟和唤醒中断。
- `Users/FreeRTOSConfig.h`：低功耗功能配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
