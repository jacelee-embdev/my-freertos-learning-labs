**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Mutex Semaphore

## Overview

This experiment protects a shared resource with a FreeRTOS mutex in a three-task scenario similar to the priority inversion demonstration. When the low-priority task owns the mutex and the high-priority task requests it, the kernel temporarily raises the owner's priority. The low-priority task can then finish its critical work and release the mutex without being delayed further by the medium-priority task. After the resource is released, the inherited priority is removed. This behavior is the mutex priority inheritance mechanism.

## Features

- Creates a mutex with `xSemaphoreCreateMutex()`.
- Constructs resource contention among low-, medium-, and high-priority tasks.
- Demonstrates priority inheritance when the high-priority task blocks.
- Provides serial logs that can be compared with experiment `012`.

## Hardware

No key interaction is required. The scheduling sequence is observed through USART1 on PA9 for TX and PA10 for RX at 115200 baud. Join the serial adapter and board grounds.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Follow the Low, Middle, and High logs, especially the point where the low-priority mutex owner runs promptly after the high-priority task begins waiting.

## Key Files

- `Drivers/BSP/freertos_test/`: Mutex and priority inheritance scenario.
- `Drivers/SYSTEM/delay/`: Simulated task load.
- `Drivers/SYSTEM/uart1/`: Task execution logs.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
