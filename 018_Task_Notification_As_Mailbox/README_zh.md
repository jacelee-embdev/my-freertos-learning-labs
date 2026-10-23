**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 任务通知模拟邮箱实验

## 项目简介

本实验把任务的 32 位通知值作为单槽邮箱使用。发送任务维护一个递增数值，每次按下 KEY1 时调用 `xTaskNotify(..., eSetValueWithOverwrite)`，把最新值写入接收任务。接收任务通过 `xTaskNotifyWait()` 阻塞读取，并在处理后延时 1 秒。由于采用覆盖写入，如果发送速度高于接收速度，中间尚未读取的旧值会被新值替换，这正体现了“只关心最新状态”的邮箱通信方式。

## 主要功能

- 直接传递一个 32 位任务通知值。
- 使用覆盖模式持续更新邮箱内容。
- 接收任务阻塞等待并在退出前清除通知位。
- 展示快速发送时旧消息被最新值覆盖的现象。

## 硬件连接

KEY1 连接 PA0，使用内部上拉并在按下时接地。USART1 使用 PA9（TX）、PA10（RX），波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。单次按 KEY1 可观察连续递增值；快速多次按下时，接收端可能跳过部分数值，只取得处理时的最新邮箱内容。

## 主要文件

- `Drivers/BSP/freertos_test/`：邮箱发送、覆盖和接收逻辑。
- `Drivers/BSP/key/`：KEY1 输入。
- `Users/FreeRTOSConfig.h`：任务通知配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
