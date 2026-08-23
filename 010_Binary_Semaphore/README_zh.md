**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 二值信号量实验

## 项目简介

本实验使用按键直观演示 FreeRTOS 二值信号量的“空”和“满”两种状态。程序创建一个初始为空的二值信号量，以及 LED 指示和按键监控两个任务。按下 KEY1 调用 `xSemaphoreGive()` 释放信号量；按下 KEY2 调用 `xSemaphoreTake()` 并最多等待 100 毫秒。信号量已满时再次释放会失败，信号量为空时获取也会超时失败，执行结果均通过串口输出。

## 主要功能

- 使用 `xSemaphoreCreateBinary()` 创建初始为空的二值信号量。
- KEY1 负责释放，KEY2 负责获取。
- 演示重复释放和空信号量获取的失败情况。
- LED1 独立闪烁，用于确认系统调度正常。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，使用内部上拉并在按下时接地。LED1 连接 PB8，低电平点亮。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。先按 KEY1 再按 KEY2可观察成功释放和获取；连续按同一个按键可观察满或空状态下的失败提示。

## 主要文件

- `Drivers/BSP/freertos_test/`：二值信号量及任务逻辑。
- `Drivers/BSP/key/`：按键扫描与消抖。
- `Users/FreeRTOSConfig.h`：信号量和调度相关配置。
