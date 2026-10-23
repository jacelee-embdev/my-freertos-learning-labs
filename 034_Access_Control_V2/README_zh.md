**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 密码门禁系统（V2，重构版）

## 项目简介

本工程保留首次设密、密码验证、修改密码、三次错误报警和 3 秒自动落锁等功能，并重构了应用层命名与职责边界。键盘扫描任务只产生输入消息，密码状态机负责设置、输入和修改模式；OLED、蜂鸣器、继电器门锁分别通过专用队列异步执行。W25Q128 访问由互斥量保护，正确、错误和报警状态通过事件组交给监控任务统一记录。

## 主要功能

- 使用清晰的输入、修改和设置三种密码状态。
- 统一消息结构与命令命名，隔离业务和硬件操作。
- 正确密码开锁并启动 3 秒单次自动落锁定时器。
- 三次连续错误触发长鸣，成功操作触发短鸣。

## 硬件连接

4×4 键盘使用 PB0–PB2、PB10、PB11–PB14；OLED SCL/SDA 接 PB8/PB9；蜂鸣器接 PB5，门锁继电器接 PB6。W25Q128 的 SPI 信号使用 PA4–PA7。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载。首次运行输入密码后按 `#` 保存；以后输入密码并按 `#` 开锁。按 `*` 进入修改流程，旧密码验证正确后输入并保存新密码。

## 主要文件

- `Drivers/BSP/app_sys_orchestrator/`：重构后的密码状态机和任务编排。
- `Drivers/BSP/password/`：密码数据处理。
- `Drivers/BSP/oled/`、`Drivers/BSP/relay_lock/`、`Drivers/BSP/w25q128/`：界面、门锁和存储。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
