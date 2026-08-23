**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 消息队列实验

## 项目简介

本实验演示 FreeRTOS 队列在任务间传递小型消息的基本用法。系统创建一个长度为 3、单项大小为 1 字节的队列，以及 LED 指示、队列接收和按键发送三个任务。按下 KEY1 或 KEY2 后，发送任务把对应键值写入队列；接收任务使用 `portMAX_DELAY` 阻塞等待，取到消息后通过 USART1 输出键值。队列为空时，接收任务不会占用 CPU 轮询。

## 主要功能

- 使用 `xQueueCreate()` 创建字节消息队列。
- 通过 `xQueueSend()` 在任务间复制发送按键值。
- 使用 `xQueueReceive()` 阻塞等待数据。
- 输出队列创建、发送和接收结果，便于观察执行流程。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。LED1 连接 PB8。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。按下任一按键，终端会依次显示队列发送成功及接收到的键值。

## 主要文件

- `Drivers/BSP/freertos_test/`：队列创建及收发任务。
- `Drivers/BSP/key/`：按键输入与消抖。
- `Drivers/SYSTEM/uart1/`：调试信息输出。
