**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Event Group Barrier

## Overview

This experiment builds a multi-task synchronization barrier with `xEventGroupSync()`. It has three logical participant roles: module 1, module 2, and a main task, represented by bit 0, bit 1, and bit 2. The main task enters the barrier and waits. KEY1 marks module 1 as ready, while KEY2 marks module 2 as ready. Only after all three event bits are present are the tasks waiting at the barrier released together, after which another synchronization cycle can begin.

## Features

- Sets each participant's ready bit while waiting for the complete mask.
- Uses KEY1 and KEY2 to simulate completion of two module initializations.
- Includes the main task as an independent barrier participant.
- Releases waiting tasks together after all conditions are satisfied.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9/PA10 at 115200 baud to display module and system synchronization results.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 and KEY2 in either order. Once the main task and both module bits are ready, the terminal reports module initialization and completion of the full barrier.

## Key Files

- `Drivers/BSP/freertos_test/`: Barrier and three logical participant roles.
- `Drivers/BSP/key/`: Module-ready event input.
- `Users/FreeRTOSConfig.h`: Event-group configuration.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
