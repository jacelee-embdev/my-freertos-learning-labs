**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# FreeRTOS LTS Source Archive

## Purpose

This directory preserves a FreeRTOS LTS source snapshot for personal embedded systems study, API reference, and porting. The archive name 202406.04 corresponds to the patch release listed in the bundled changelog; the kernel headers identify V11.1.0. Check [manifest.yml](FreeRTOS-LTS/manifest.yml) together with [CHANGELOG.md](FreeRTOS-LTS/CHANGELOG.md) for component versions.

## Contents

- `FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/`: Kernel sources, public headers, processor ports, and memory allocation implementations.
- `FreeRTOS-LTS/FreeRTOS/`: Also contains FreeRTOS-Plus-TCP, coreMQTT, coreHTTP, coreJSON, corePKCS11, coreSNTP, and related components.
- `FreeRTOS-LTS/aws/`: Companion libraries for device shadows, device management, jobs, fleet provisioning, and MQTT file streams.

## Reading and Use

Start with [include/task.h](FreeRTOS-LTS/FreeRTOS/FreeRTOS-Kernel/include/task.h) alongside `tasks.c`, then explore queues, event groups, and software timers. For a port, select the appropriate processor/compiler directory under `portable`, choose a suitable `heap_*.c`, and provide `FreeRTOSConfig.h` in the application.

This directory serves as a source reference and does not contain a standalone STM32 Keil application. See project 001 and the later experiments for board-level behavior; each experiment uses its own kernel copy and project configuration.

The added archive documentation uses the root `LICENSE`. Upstream sources, tests, original documentation, and copyright notices are preserved, and each component retains its own license; see the [upstream license](FreeRTOS-LTS/LICENSE.md).

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
