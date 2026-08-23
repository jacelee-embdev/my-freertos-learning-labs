**简体中文** | [English](README.md)

# STM32F103C8T6 任务通知模拟事件标志组实验

## 项目简介

本实验使用任务通知的 `eSetBits` 模式模拟事件标志位。KEY1 将接收任务通知值的 bit0 置位，KEY2 将 bit1 置位。接收任务通过 `xTaskNotifyWait()` 获取并清除本次通知位，再把结果累计到本地变量中；当 bit0 和 bit1 都出现后，任务输出组合事件成功并清空累计值。该方法适合单一接收任务的轻量级多事件通知。

## 主要功能

- 使用一个 32 位通知值承载多个事件位。
- KEY1 和 KEY2 分别产生 bit0、bit1 事件。
- 接收端自行累计不同批次到达的事件。
- 两个事件均满足后执行组合处理并重新开始。

## 硬件连接

KEY1、KEY2 分别连接 PA0、PA1，采用内部上拉并在按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。以任意顺序按下 KEY1 和 KEY2；当两类事件都被接收后，终端显示组合值并进入下一轮等待。

## 主要文件

- `Drivers/BSP/freertos_test/`：事件位发送、接收与累计。
- `Drivers/BSP/key/`：双按键事件源。
- `Users/FreeRTOSConfig.h`：任务通知配置。
