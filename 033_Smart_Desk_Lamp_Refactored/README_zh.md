**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 智能台灯系统（重构版）

## 项目简介

本工程保留智能台灯的三种工作模式、自动调光、测距报警、久坐提醒和蓝牙控制，并重构了状态命名、阈值配置和初始化流程。传感任务集中更新人体、光照、距离及报警位；设备任务根据共享状态驱动 PWM 灯和蜂鸣器；显示、按键和蓝牙各自由独立任务负责。全局状态通过互斥量保护，初始化时统一清零，报警条件通过事件组传递。

## 主要功能

- 自动模式根据人体存在和环境光选择灯光档位。
- 按键模式支持四档亮度，远程模式接收 `0`–`6` 命令。
- 距离小于 5 cm 持续 5 秒或计时约 10 秒触发报警。
- OLED 实时显示模式、人体状态、距离和使用时间。

## 硬件连接

PWM 灯接 PB0，光照输入接 PA1，人体传感器接 PB12，HC-SR04 接 PA11/PA12。四个按键接 PA4–PA7，OLED 接 PB6/PB7，蜂鸣器接 PB13，蓝牙 USART2 接 PA2/PA3。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载。KEY1 循环切换自动、按键、远程模式；KEY2 调节亮度，KEY3 控制定时，KEY4 清零。远程模式可从蓝牙发送单字符命令。

## 主要文件

- `Drivers/BSP/app_sys_orchestrator/`：重构后的共享状态和任务协作。
- `Drivers/BSP/light_sensor/`、`Drivers/BSP/hcsr04/`、`Drivers/BSP/ia_sensor/`：传感模块。
- `Drivers/BSP/led/`、`Drivers/BSP/bluetooth/`、`Drivers/BSP/oled/`：灯光、蓝牙和显示驱动。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
