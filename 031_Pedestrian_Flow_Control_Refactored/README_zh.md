**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 人员通行流量控制系统（重构版）

## 项目简介

本工程保留原版的五人限流、闸机控制和 3 秒自动恢复功能，并将应用层整理为传感器状态机、闸机执行器和独立显示任务。红外事件消耗计数型信号量中的通行名额；名额用尽后，状态队列通知执行器关闭 PB6 闸机、点亮警示状态并启动蜂鸣器。软件定时器到期后使用任务通知唤醒传感器任务，由它统一重置信号量、状态和 LCD 内容。

## 主要功能

- 以明确的 `ALLOW_PASS` 和 `LOCKED_WAIT` 状态组织业务流程。
- 使用状态队列和显示队列隔离业务、执行器与 UI。
- 使用 LCD 互斥量防止非重入显示操作冲突。
- 对锁定期间的传感器标志和通行名额执行完整复位。

## 硬件连接

红外传感器接 PB4，蜂鸣器接 PB5，闸机继电器接 PB6，LED1/LED2 接 PB8/PB9。LCD1602 的 D0–D7 接 PA0–PA7，RS/RW/EN 接 PB1/PB2/PB10。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载。连续触发 5 次红外传感器，观察 LCD 计数和闸机状态；系统锁定报警约 3 秒后会清除旧触发并恢复 `PASS...00/05`。

## 主要文件

- `Drivers/BSP/app_sys_orchestrator/`：重构后的状态机和任务编排。
- `Drivers/BSP/exti/`：红外事件采集。
- `Drivers/BSP/lcd1602/`、`Drivers/BSP/relay_gate/`：显示与执行器驱动。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
