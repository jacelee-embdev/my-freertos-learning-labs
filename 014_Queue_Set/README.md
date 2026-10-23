**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Queue Set

## Overview

This experiment demonstrates how one FreeRTOS task can wait for several different event sources through a queue set. The application creates a one-entry data queue and a binary semaphore, then adds both objects to a queue set with a capacity of two. KEY1 writes an incrementing value to the data queue, while KEY2 gives the semaphore. The processing task blocks on a single `xQueueSelectFromSet()` call, identifies which member became ready, and performs the matching receive or take operation.

## Features

- Creates a queue set with `xQueueCreateSet()`.
- Registers a normal queue and a binary semaphore as members.
- Produces two event types through KEY1 and KEY2.
- Waits efficiently and identifies the ready source with `xQueueSelectFromSet()`.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and become active when grounded. USART1 uses PA9/PA10 at 115200 baud to display queue data and semaphore events.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 to receive incrementing queue values and KEY2 to receive semaphore events. The consumer task automatically distinguishes the source.

## Key Files

- `Drivers/BSP/freertos_test/`: Queue set, member objects, and handler task.
- `Drivers/BSP/key/`: Event-trigger keys.
- `Users/FreeRTOSConfig.h`: Queue-set feature configuration.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
