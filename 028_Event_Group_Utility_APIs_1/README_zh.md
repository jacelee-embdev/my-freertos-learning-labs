**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 事件组状态查询实验

## 项目简介

本实验演示如何设置并非破坏性地读取 FreeRTOS 事件组当前状态。KEY1 将 bit0 置位，KEY2 将 bit1 置位；状态监控任务每秒调用 `xEventGroupGetBits()` 读取全部事件位，并分别打印已经发生的按键事件。该 API 只读取而不清除标志，因此位一旦被设置就会持续保留，后续监控周期仍能看到相同状态。

## 主要功能

- 使用 `xEventGroupSetBits()` 设置独立事件位。
- 使用 `xEventGroupGetBits()` 获取当前状态快照。
- 分别解析 bit0 和 bit1，可同时报告多个事件。
- 展示读取操作不会自动清除事件位的特点。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，采用内部上拉并在按下时接地。USART1 使用 PA9（TX）、PA10（RX），波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。按下 KEY1 或 KEY2 后等待最多约 1 秒，终端会持续报告对应标志；两个按键都按过后，两类事件都会持续显示。

## 主要文件

- `Drivers/BSP/freertos_test/`：事件位设置和状态读取。
- `Drivers/BSP/key/`：双按键事件输入。
- `Users/FreeRTOSConfig.h`：事件组配置。
