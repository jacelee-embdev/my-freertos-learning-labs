**简体中文** | [English](README.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS LTS 源码归档

## 目录用途

本目录保存用于个人嵌入式学习的 FreeRTOS LTS 源码快照，便于查阅内核实现、对照接口和准备移植。归档名称为 202406.04，对应随附更新记录中的补丁版本；内核头文件标识为 V11.1.0。组件版本可结合 [manifest.yml](FreeRTOS-LTS/manifest.yml) 和 [CHANGELOG.md](FreeRTOS-LTS/CHANGELOG.md) 核对。

## 内容结构

- `FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/`：内核源码、公开头文件、处理器移植层和内存分配实现。
- `FreeRTOS-LTS/FreeRTOS/`：同时保存 FreeRTOS-Plus-TCP、coreMQTT、coreHTTP、coreJSON、corePKCS11、coreSNTP 等组件。
- `FreeRTOS-LTS/aws/`：保存设备影子、设备管理、AWS IoT Jobs、设备预置及 MQTT 文件流等配套组件。

## 阅读与使用

阅读内核时，可先从 [include/task.h](FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include/task.h) 对照 `tasks.c`，再查看队列、事件组和软件定时器实现。移植时应按处理器与编译器选择 `portable` 下的端口，选择合适的 `heap_*.c`，并在具体应用中配置 `FreeRTOSConfig.h`。

此目录用于源码查阅和移植参考，没有独立的 STM32 Keil 应用工程。板级运行现象请结合本仓库 001 及后续实验的说明；各实验使用自己目录中的内核副本和工程配置。

本目录新增的归档说明采用根目录 `LICENSE`；上游源码、测试、原始文档和版权声明保持原样，各组件遵循其自带许可证，参见 [上游许可证](FreeRTOS-LTS/LICENSE.md)。

## 许可证

本项目采用 **MIT License** 开源许可证。Copyright (c) 2026 JesMicro。完整条款请参阅根目录的 [LICENSE](LICENSE) 文件。第三方组件遵循各自的许可证。
