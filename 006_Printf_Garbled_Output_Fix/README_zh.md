**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 多任务串口打印保护实验

## 项目简介

本实验针对多个 FreeRTOS 任务同时调用 `printf()` 时可能出现的字符交叉、数据断裂或乱码问题，提供一个简单的调试打印保护方案。`debug_printf()` 宏在格式化输出前进入临界区，输出完成后退出临界区，使一条调试消息不会被其他任务或中断打断。三个任务在启动时分别打印运行状态，同时继续执行双 LED 闪烁和 KEY1 删除 LED1 任务的测试逻辑。

## 主要功能

- 使用 `taskENTER_CRITICAL()` 和 `taskEXIT_CRITICAL()` 包裹调试输出。
- 支持与标准 `printf()` 相同的可变参数格式。
- 演示多个同优先级任务的串口输出保护。
- 通过 KEY1 删除 LED1 任务并打印状态。

## 硬件连接

USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200。LED1、LED2 分别连接 PB8、PB9；KEY1 连接 PA0并在按下时接地。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载后连接串口终端。复位时可观察各任务的完整启动消息。临界区会暂时屏蔽调度和中断，因此该方案仅适合低频、短消息调试。

## 主要文件

- `Drivers/SYSTEM/uart1/uart1.h`：`debug_printf()` 临界区宏。
- `Drivers/BSP/freertos_test/`：并发打印及任务测试。
- `Drivers/SYSTEM/uart1/`：USART1 驱动与 `printf()` 重定向。
