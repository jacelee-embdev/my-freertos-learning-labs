**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 动态任务创建实验

## 项目简介

本实验演示 FreeRTOS 使用动态内存创建和管理任务的方法。程序通过 `xTaskCreate()` 创建 LED1、LED2 和按键监控三个任务，任务栈与任务控制块由 FreeRTOS 堆自动分配。两个 LED 任务分别以 1 秒和 500 毫秒为间隔闪烁；按下 KEY1 后，监控任务调用 `vTaskDelete()` 删除 LED1 任务，并将对应句柄清空，避免后续访问已经失效的任务对象。

## 主要功能

- 启用 FreeRTOS 动态内存分配并创建三个同优先级任务。
- 使用 `pdMS_TO_TICKS()` 将毫秒时间转换为系统节拍。
- 通过任务句柄动态删除 LED1 任务。
- 重复按下 KEY1 时，通过 USART1 提示任务已经不存在。

## 硬件连接

LED1 和 LED2 分别连接 PB8、PB9，均为低电平点亮。KEY1 连接 PA0，配置为内部上拉输入，按下时接地。USART1 使用 PA9/PA10，波特率为 115200。

## 使用方法

使用 Keil MDK 打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译并下载。复位后两个 LED 持续闪烁；按下 KEY1 后，LED1 停止闪烁，LED2 和按键监控任务继续运行。

## 主要文件

- `Drivers/BSP/freertos_test/`：动态任务创建、运行与删除逻辑。
- `Drivers/BSP/key/`：按键初始化、扫描和消抖。
- `Users/FreeRTOSConfig.h`：动态分配及调度配置。
