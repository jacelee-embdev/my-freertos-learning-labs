**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 Task Notification as Counting Semaphore

## Overview

This experiment uses the increment and decrement behavior of a task notification value to model a counting semaphore. Every KEY1 press makes the sender call `xTaskNotifyGive()`, increasing the receiver's notification value by one. The receiver waits with `ulTaskNotifyTake(pdFALSE, portMAX_DELAY)`. The `pdFALSE` argument decrements the value by one for each receive instead of clearing it completely. Because the receiver delays for one second after processing, several rapid key presses can build a backlog whose pre-decrement count is visible in the serial log.

## Features

- Accumulates multiple key events in a task notification value.
- Consumes exactly one notification count per receive.
- Blocks efficiently while no event is pending.
- Uses a slow consumer to demonstrate queued notification counts.

## Hardware

KEY1 uses PA0 with an internal pull-up and is active when grounded. USART1 uses PA9/PA10 at 115200 baud to display notification increments and consumption.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 rapidly several times. The receiver processes one notification about every second and reports the number of pending events before each decrement.

## Key Files

- `Drivers/BSP/freertos_test/`: Counting notification behavior.
- `Drivers/BSP/key/`: Key event source.
- `Users/FreeRTOSConfig.h`: Task notification settings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
