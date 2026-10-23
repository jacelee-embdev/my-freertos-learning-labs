**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Counting Semaphore

## Overview

This experiment demonstrates how a counting semaphore can represent several identical resources or accumulated events. The application calls `xSemaphoreCreateCounting(5, 0)` to create a semaphore with a maximum count of five and an initial count of zero. KEY1 gives the semaphore and increases the available count. KEY2 attempts to take one count with a 100-millisecond timeout. After every successful operation, the current value is read and printed, making it easy to observe the count moving between zero and five.

## Features

- Creates a counting semaphore with a capacity of five.
- Uses KEY1 to add resources and KEY2 to consume them.
- Reads the current value with `uxSemaphoreGetCount()`.
- Demonstrates failed operations at the upper and lower limits.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and become active when grounded. LED1 uses PB8. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 repeatedly to raise the count to five. Press KEY2 to consume the stored counts one at a time while watching the reported value.

## Key Files

- `Drivers/BSP/freertos_test/`: Counting semaphore creation and operations.
- `Drivers/BSP/key/`: Key input.
- `Drivers/SYSTEM/uart1/`: Count and result output.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
