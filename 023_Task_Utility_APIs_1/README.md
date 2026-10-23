**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Task Utility APIs I

## Overview

This experiment collects several FreeRTOS APIs related to task priorities, task handles, and task suspension. Task 1 first reads the initial priority of task 2 and changes it from 1 to 5. It then looks up task 2 by name and suspends it through the returned handle. Finally, task 1 obtains its own handle with `xTaskGetCurrentTaskHandle()` and suspends itself. Serial messages show the sequence and demonstrate that handles can be stored during creation or queried later at runtime.

## Features

- Reads a task priority with `uxTaskPriorityGet()`.
- Changes a priority dynamically with `vTaskPrioritySet()`.
- Finds a handle by task name with `xTaskGetHandle()`.
- Obtains the current task handle and demonstrates self-suspension.

## Hardware

The experiment is observed mainly through USART1 on PA9 for TX and PA10 for RX at 115200 baud. PA0 and PA1 keys are initialized, but task 2 is suspended early, so the central demonstration requires no key input.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Reset the board and follow the logs for priority reading, priority modification, name-based handle lookup, and suspension of both application tasks.

## Key Files

- `Drivers/BSP/freertos_test/`: Task utility API sequence.
- `Users/FreeRTOSConfig.h`: Optional task API settings.
- `Drivers/SYSTEM/uart1/`: Test result output.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
