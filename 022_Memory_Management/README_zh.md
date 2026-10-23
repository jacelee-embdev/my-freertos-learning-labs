**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 内存管理实验

## 项目简介

本实验演示 FreeRTOS 堆内存的申请、释放和剩余空间查询。工程使用 `heap_4.c`，KEY1 调用 `pvPortMalloc(30)` 申请 30 字节，KEY2 调用 `vPortFree()` 释放当前缓冲区并把指针置空。另一个任务每 2 秒调用 `xPortGetFreeHeapSize()` 输出剩余堆空间，可观察创建任务和动态分配对 FreeRTOS 堆的影响。

## 主要功能

- 使用 FreeRTOS 内存接口申请和释放 30 字节缓冲区。
- 释放后清空指针，避免重复释放和野指针访问。
- 周期显示当前剩余堆内存。
- 通过串口报告申请、释放成功或失败。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，采用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。按 KEY1 申请内存，再按 KEY2 释放。请避免未释放时连续按 KEY1，否则旧指针会被覆盖并造成无法回收的内存泄漏。

## 主要文件

- `Drivers/BSP/freertos_test/`：内存申请、释放和监控任务。
- `Middlewares/FreeRTOS/portable/MemMang/heap_4.c`：堆实现。
- `Users/FreeRTOSConfig.h`：FreeRTOS 堆大小配置。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
