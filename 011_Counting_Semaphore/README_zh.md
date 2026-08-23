**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 计数型信号量实验

## 项目简介

本实验演示计数型信号量如何表示多个同类资源或累计事件。程序通过 `xSemaphoreCreateCounting(5, 0)` 创建最大计数为 5、初始计数为 0 的信号量。按下 KEY1 会释放一次信号量并增加计数，按下 KEY2 会在 100 毫秒内尝试获取一次并减少计数。串口在每次成功操作后显示当前剩余值，因此可以直接观察计数在 0 到 5 之间变化。

## 主要功能

- 创建容量为 5 的计数型信号量。
- 使用 KEY1 累加可用资源，KEY2 消耗资源。
- 通过 `uxSemaphoreGetCount()` 读取并显示当前计数。
- 演示达到上限或降到零后的操作失败情况。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，采用内部上拉并在按下时接地。LED1 连接 PB8。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。多次按 KEY1 可把计数增加到 5，再按 KEY2逐次消耗；终端会显示每次操作后的计数值。

## 主要文件

- `Drivers/BSP/freertos_test/`：计数型信号量创建与操作。
- `Drivers/BSP/key/`：按键输入。
- `Drivers/SYSTEM/uart1/`：计数和结果输出。
