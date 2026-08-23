**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 中断恢复任务实验

## 项目简介

本实验演示任务被挂起后，如何从外部中断服务程序中安全恢复。程序运行 LED1、LED2 和按键监控任务。按下 KEY1 时，监控任务调用 `vTaskSuspend()` 挂起 LED1；KEY2 所在的 PA1 配置为下降沿 EXTI1 中断，按下后回调函数使用 `xTaskResumeFromISR()` 恢复 LED1。若恢复的任务需要立即运行，程序通过 `portYIELD_FROM_ISR()` 在中断退出时请求上下文切换。

## 主要功能

- 在普通任务中挂起 LED1 任务。
- 通过 PA1 外部中断恢复被挂起的任务。
- 正确使用 FreeRTOS 的 FromISR API 和中断让步宏。
- 保留 LED2 作为调度器持续运行的状态指示。

## 硬件连接

LED1、LED2 分别连接 PB8、PB9。KEY1 连接 PA0，用于挂起任务；KEY2 连接 PA1，用于触发下降沿外部中断。两个按键均使用内部上拉并在按下时接地。USART1 使用 PA9/PA10。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译并下载。按 KEY1 挂起 LED1，再按 KEY2 触发 EXTI1 并恢复 LED1，通过串口可查看恢复信息。

## 主要文件

- `Drivers/BSP/freertos_test/`：任务创建与挂起逻辑。
- `Drivers/BSP/exti/`：EXTI1 配置及中断恢复处理。
- `Users/main.c`：外设初始化与调度器入口。
