**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 队列传递大数据实验

## 项目简介

本实验展示通过 FreeRTOS 队列传递大块数据时常用的指针传递方式。程序准备一个静态的大型字符缓冲区，但队列项类型定义为 `char *`，因此按键任务写入队列的只是缓冲区地址，而不是复制整段文本。接收任务阻塞等待指针，取出后通过 USART1 打印其指向的字符串。这种方法可以显著减少队列内存和复制开销，但要求发送后缓冲区仍然有效，并由应用保证并发访问安全。

## 主要功能

- 创建长度为 3、队列项为字符指针的消息队列。
- 按下任一按键后发送静态消息缓冲区地址。
- 接收任务通过指针访问并打印完整文本。
- 对比小数据复制与大数据零拷贝式传递思路。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉。LED1 连接 PB8。USART1 使用 PA9/PA10，波特率为 115200，用于显示长文本。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。按下任一按键后，可观察发送任务和接收任务打印同一段大数据内容。

## 主要文件

- `Drivers/BSP/freertos_test/freertos_test.h`：指针队列项类型定义。
- `Drivers/BSP/freertos_test/freertos_test.c`：缓冲区及队列收发任务。
- `Drivers/SYSTEM/uart1/`：长文本调试输出。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
