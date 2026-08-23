**简体中文** | [English](README.md)

# STM32F103C8T6 FreeRTOS 静态任务创建实验

## 项目简介

本实验演示如何在不依赖 FreeRTOS 堆为应用任务分配内存的情况下创建任务。LED1、LED2 和按键监控任务均通过 `xTaskCreateStatic()` 创建，每个任务使用预先定义的 `StackType_t` 栈数组和 `StaticTask_t` 任务控制块。显式管理内存可以让 RAM 占用和任务生命周期更加确定，适合对内存布局与运行可靠性要求较高的嵌入式系统。

## 主要功能

- 使用静态栈和静态任务控制块创建三个应用任务。
- 实现 `vApplicationGetIdleTaskMemory()`，为内核空闲任务提供内存。
- 实现 `vApplicationGetTimerTaskMemory()`，为软件定时器服务任务提供内存。
- 两个 LED 以不同周期闪烁，按下 KEY1 可删除 LED1 任务。

## 硬件连接

LED1 和 LED2 分别连接 PB8、PB9，均为低电平点亮。KEY1 连接 PA0，使用内部上拉，按下时接地。USART1 使用 PA9/PA10，波特率为 115200，用于输出任务删除信息。

## 使用方法

使用 Keil MDK 打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译并下载。复位后观察两个 LED 闪烁；按下 KEY1 后，LED1 任务被删除，其他任务继续调度。

## 主要文件

- `Drivers/BSP/freertos_test/`：静态任务、栈、TCB 及内核内存回调。
- `Users/FreeRTOSConfig.h`：静态分配和软件定时器配置。
- `Users/main.c`：外设初始化与调度器启动。
