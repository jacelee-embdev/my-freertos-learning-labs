**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 智能台灯系统（原版）

## 项目简介

本综合实验实现带人体感应、环境光调节、超声波测距、久坐提醒和蓝牙控制的智能台灯。系统提供自动、按键和远程三种模式。自动模式下，有人时根据光照强度选择高、中、低亮度，无人时关灯；距离持续小于 5 cm 达 5 秒，或连续使用达到约 10 秒，蜂鸣器会报警。OLED 显示工作模式、人体状态、距离和计时数据。

## 主要功能

- KEY1 切换模式，KEY2 在按键模式下调节四档亮度。
- KEY3 启停计时，KEY4 停止并清零计时。
- 蓝牙远程模式支持亮度和计时命令 `0`–`6`。
- 使用互斥量保护共享状态，事件组管理两类报警。

## 硬件连接

PWM 灯接 PB0，光照传感器接 PA1，人体传感器接 PB12，HC-SR04 的 TRIG/ECHO 接 PA11/PA12。按键接 PA4–PA7，OLED SCL/SDA 接 PB6/PB7，蜂鸣器接 PB13，蓝牙 USART2 接 PA2/PA3。

## 使用方法

打开 `Projects/MDK-ARM/STM32F103.uvprojx`，编译下载。使用 KEY1 选择模式；自动模式下改变人体和光照输入，按键模式用 KEY2 调光，远程模式从蓝牙发送字符命令。

## 主要文件

- `Drivers/BSP/app_sys_orchestrator/`：原版传感、显示和控制任务。
- `Drivers/BSP/light_sensor/`、`Drivers/BSP/hcsr04/`、`Drivers/BSP/ia_sensor/`：环境采集。
- `Drivers/BSP/led/`、`Drivers/BSP/bluetooth/`、`Drivers/BSP/oled/`：灯光、远程控制和界面。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
