**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 堆统计 API 实验

## 项目简介

本实验在基本内存申请和释放操作上增加 FreeRTOS 堆统计查询。工程采用 `heap_4.c`，KEY1 申请 30 字节，KEY2 释放当前缓冲区。监控任务每 2 秒调用 `vPortGetHeapStats()`，输出当前空闲空间、最大连续空闲块、累计成功分配次数和释放次数，同时使用 `xPortGetFreeHeapSize()` 再次报告总空闲大小。通过这些数据可以初步观察堆使用和碎片变化。

## 主要功能

- 使用 `HeapStats_t` 获取详细堆状态。
- 显示空闲空间和最大连续可分配块。
- 统计成功的内存分配和释放次数。
- 结合按键操作实时观察统计值变化。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。交替按 KEY1 和 KEY2 观察统计变化。不要在尚未释放时重复申请，否则保存的旧指针会丢失并形成内存泄漏。

## 主要文件

- `Drivers/BSP/freertos_test/`：堆操作与统计监控。
- `Middlewares/FreeRTOS/portable/MemMang/heap_4.c`：堆实现。
- `Users/FreeRTOSConfig.h`：堆大小和内存配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
