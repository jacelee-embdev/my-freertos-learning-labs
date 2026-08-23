**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 队列辅助 API 实验

## 项目简介

本实验在基本队列收发之外，演示队列剩余容量、待处理消息数和复位操作。程序创建一个长度为 3 的字节队列。按下 KEY1 向队列写入键值，并通过 `uxQueueSpacesAvailable()` 输出剩余槽位；当队列刚好填满时，程序立即调用 `xQueueReset()` 清空队列。按下 KEY2 尝试接收一条消息，并使用 `uxQueueMessagesWaiting()` 显示剩余消息数量。

## 主要功能

- 查询队列当前剩余可写槽位。
- 查询队列中等待接收的消息数。
- 在队列填满时演示 `xQueueReset()` 清空操作。
- 使用 LED1 作为独立运行心跳指示。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。LED1 连接 PB8。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。按 KEY1 写入队列并观察容量递减；第三次写入后队列自动复位。可在复位前按 KEY2 读取消息并查看剩余数量。

## 主要文件

- `Drivers/BSP/freertos_test/`：队列状态查询和复位逻辑。
- `Drivers/BSP/key/`：队列操作输入。
- `Drivers/BSP/led/`：系统心跳指示。
