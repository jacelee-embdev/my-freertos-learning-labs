**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Queue Utility APIs

## Overview

This experiment extends basic queue send and receive operations with queries for free capacity, pending messages, and queue reset. The application creates a three-entry byte queue. KEY1 writes its key value and uses `uxQueueSpacesAvailable()` to print the remaining slots. When the third entry fills the queue, the application immediately calls `xQueueReset()` to clear it. KEY2 attempts to receive one item and reports the number still waiting through `uxQueueMessagesWaiting()`.

## Features

- Queries the number of currently available queue slots.
- Reads the number of messages waiting for a receiver.
- Demonstrates `xQueueReset()` when the queue becomes full.
- Runs LED1 as an independent system heartbeat.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and become active when grounded. LED1 uses PB8. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 to add messages and watch the free capacity decrease. The third send resets the full queue. Press KEY2 before that reset to receive an item and inspect the remaining count.

## Key Files

- `Drivers/BSP/freertos_test/`: Queue queries and reset logic.
- `Drivers/BSP/key/`: Queue operation input.
- `Drivers/BSP/led/`: System heartbeat.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
