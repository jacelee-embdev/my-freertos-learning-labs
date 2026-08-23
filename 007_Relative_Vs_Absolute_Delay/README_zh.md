**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 相对延时与绝对延时实验

## 项目简介

本实验对比 `vTaskDelay()` 相对延时和 `xTaskDelayUntil()` 绝对延时对周期任务的影响。两个任务每轮都先翻转一个 LED，并用 20 毫秒忙等待模拟业务处理。LED1 任务随后相对延时 500 毫秒，因此实际周期约为 520 毫秒；LED2 任务根据上次唤醒时间执行绝对延时，使总体周期保持在约 500 毫秒，避免处理时间不断累积形成漂移。

## 主要功能

- 展示相对延时会把任务执行时间计入下一周期。
- 使用 `xTaskGetTickCount()` 建立绝对周期基准。
- 通过 `xTaskDelayUntil()` 保持稳定的周期唤醒时间。
- 当任务处理时间超过周期预算时输出过载提示。

## 硬件连接

LED1 连接 PB8，用于观察相对延时；LED2 连接 PB9，用于观察绝对延时。两者均为低电平点亮。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载后同时观察两个 LED。长时间运行时，LED1 相对 LED2 会逐渐产生相位偏移。串口用于显示可能的任务过载信息。

## 主要文件

- `Drivers/BSP/freertos_test/`：两种延时方式的任务实现。
- `Drivers/BSP/led/`：双 LED 驱动。
- `Drivers/SYSTEM/delay/`：模拟业务耗时的忙等待函数。
