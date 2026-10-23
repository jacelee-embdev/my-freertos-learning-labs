**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS 互斥量持有者查询实验

## 项目简介

本实验在互斥量优先级继承场景中加入持有者查询。低优先级任务先取得互斥量并执行耗时操作；高优先级任务准备获取资源时，先调用 `xSemaphoreGetMutexHolder()` 获得当前持有任务的句柄，再使用 `pcTaskGetName()` 输出持有者名称。随后高优先级任务阻塞等待互斥量，并触发 FreeRTOS 的优先级继承机制。

## 主要功能

- 使用互斥量保护模拟共享资源。
- 查询互斥量当前是否被任务锁定。
- 从持有者句柄获取并打印任务名称。
- 结合低、中、高优先级任务观察优先级继承。

## 硬件连接

本实验无需按键操作，主要通过 USART1 查看任务日志。USART1 使用 PA9（TX）和 PA10（RX），波特率为 115200，串口模块与开发板需要共地。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载并连接串口。复位后观察 High 任务在请求互斥量前打印当前持有者 `task_low`，随后查看资源交接和优先级继承过程。

## 主要文件

- `Drivers/BSP/freertos_test/`：互斥量竞争和持有者查询。
- `Drivers/SYSTEM/delay/`：模拟任务负载。
- `Drivers/SYSTEM/uart1/`：持有者与调度日志。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
