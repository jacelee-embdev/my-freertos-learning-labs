**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 事件标志组实验

## 项目简介

本实验使用 FreeRTOS 事件组同步多个独立事件。KEY1 对应 bit0，KEY2 对应 bit1，按键任务通过 `xEventGroupSetBits()` 设置相应标志。事件处理任务调用 `xEventGroupWaitBits()` 阻塞等待，并配置为逻辑与条件，因此只有两个标志都被置位后才会继续运行。条件满足时两个标志自动清零，系统随后进入下一轮等待。

## 主要功能

- 使用 `xEventGroupCreate()` 创建事件标志组。
- 通过两个按键分别设置 bit0 和 bit1。
- 以“等待全部标志”的方式同步两个事件。
- 条件满足后自动清除标志并打印返回位值。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。KEY1 和 KEY2 的按下顺序不限；只有两者都按过一次后，终端才会显示事件等待完成及返回的标志值。

## 主要文件

- `Drivers/BSP/freertos_test/`：事件位设置与等待任务。
- `Drivers/BSP/key/`：双按键输入。
- `Users/FreeRTOSConfig.h`：事件组相关内核配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
