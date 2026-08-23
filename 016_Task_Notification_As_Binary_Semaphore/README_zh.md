**简体中文** | [English](README.md)

# STM32F103C8T6 任务通知模拟二值信号量实验

## 项目简介

本实验使用 FreeRTOS 任务通知实现类似二值信号量的单向事件同步。按键任务检测到 KEY1 后，通过 `xTaskNotifyGive()` 向接收任务发送通知；接收任务调用 `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)` 阻塞等待。参数 `pdTRUE` 会在成功接收后把通知值清零，使多个尚未处理的通知合并成一次有效事件，表现出接近二值信号量的使用方式。

## 主要功能

- 不创建独立信号量对象，直接使用任务内置通知值。
- KEY1 触发发送任务向指定接收任务通知。
- 接收任务无事件时阻塞，不进行轮询。
- 接收后清零通知值，模拟二值状态。

## 硬件连接

KEY1 连接 PA0，配置为内部上拉输入，按下时接地。USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200，用于显示通知的发送和接收过程。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口终端。每次按下 KEY1，终端会先显示通知发送，再显示接收任务被唤醒并成功获取事件。

## 主要文件

- `Drivers/BSP/freertos_test/`：任务通知发送与接收逻辑。
- `Drivers/BSP/key/`：KEY1 扫描与消抖。
- `Users/FreeRTOSConfig.h`：任务通知功能配置。
