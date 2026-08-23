**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 队列集实验

## 项目简介

本实验演示一个任务如何通过 FreeRTOS 队列集同时等待不同类型的事件源。程序创建一个长度为 1 的数据队列和一个二值信号量，并把两者加入容量为 2 的队列集。KEY1 向数据队列写入递增数值，KEY2 释放二值信号量。处理任务只需阻塞调用一次 `xQueueSelectFromSet()`，即可判断是哪一个成员就绪并执行对应的接收或获取操作。

## 主要功能

- 使用 `xQueueCreateSet()` 创建队列集。
- 将普通队列和二值信号量注册为队列集成员。
- 通过 KEY1、KEY2 产生两种不同事件。
- 使用 `xQueueSelectFromSet()` 统一阻塞等待并识别来源。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，采用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200，用于显示队列数据和信号量事件。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。按 KEY1 可看到递增队列数据；按 KEY2 可看到信号量事件。接收任务会自动区分事件来源。

## 主要文件

- `Drivers/BSP/freertos_test/`：队列集、成员资源和处理任务。
- `Drivers/BSP/key/`：事件触发按键。
- `Users/FreeRTOSConfig.h`：队列集功能配置。
