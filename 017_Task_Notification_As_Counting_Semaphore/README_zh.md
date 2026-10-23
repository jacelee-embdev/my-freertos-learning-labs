**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 任务通知模拟计数型信号量实验

## 项目简介

本实验利用任务通知值的递增和递减行为模拟计数型信号量。每次按下 KEY1，发送任务调用 `xTaskNotifyGive()`，使接收任务的通知值加一。接收任务通过 `ulTaskNotifyTake(pdFALSE, portMAX_DELAY)` 获取通知；`pdFALSE` 表示每次只将计数减一，而不是全部清零。接收任务每处理一次后延时 1 秒，因此快速连续按键可以形成待处理事件积压，并从串口观察提取前的计数。

## 主要功能

- 使用任务通知值累计多次按键事件。
- 每次接收只消耗一个通知计数。
- 阻塞等待事件并避免空闲轮询。
- 通过慢速接收任务展示事件积压效果。

## 硬件连接

KEY1 连接 PA0，采用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200，用于显示通知计数的递增和提取。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。快速连续按下 KEY1，接收任务将以约 1 秒间隔逐个处理通知，终端会显示每次提取前积压的事件数。

## 主要文件

- `Drivers/BSP/freertos_test/`：计数型任务通知实验逻辑。
- `Drivers/BSP/key/`：按键事件源。
- `Users/FreeRTOSConfig.h`：任务通知配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
