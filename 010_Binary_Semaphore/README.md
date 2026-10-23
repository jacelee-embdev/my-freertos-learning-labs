**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Binary Semaphore

## Overview

This experiment uses two keys to demonstrate the empty and available states of a FreeRTOS binary semaphore. The application creates an initially empty semaphore, an LED indicator task, and a key-monitor task. KEY1 calls `xSemaphoreGive()` to make the semaphore available. KEY2 calls `xSemaphoreTake()` with a timeout of 100 milliseconds. Giving an already available binary semaphore fails, and taking an empty semaphore times out. Every result is printed to the serial terminal so the state transitions can be followed directly.

## Features

- Creates an initially empty semaphore with `xSemaphoreCreateBinary()`.
- Uses KEY1 to give and KEY2 to take the semaphore.
- Demonstrates failure when giving a full or taking an empty semaphore.
- Keeps LED1 blinking independently to confirm scheduler activity.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. LED1 uses PB8 and is active low. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, then build, flash, and connect a serial terminal. Press KEY1 followed by KEY2 to observe a successful give and take. Repeatedly pressing either key demonstrates the full and empty failure cases.

## Key Files

- `Drivers/BSP/freertos_test/`: Binary semaphore and task logic.
- `Drivers/BSP/key/`: Key scanning and debouncing.
- `Users/FreeRTOSConfig.h`: Semaphore and scheduler settings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
