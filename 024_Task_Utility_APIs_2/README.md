**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Task Utility APIs II

## Overview

This experiment demonstrates how to inspect the number, state, and stack usage of tasks in a running FreeRTOS system. At startup, task 1 calls `vTaskList()` to print a formatted task table. It then reads the current task count, allocates a matching `TaskStatus_t` array, and fills it with `uxTaskGetSystemState()`. Finally, it finds task 1 by name and calls `vTaskGetInfo()` to obtain its priority, runtime counter, and stack high-water mark before releasing the temporary array.

## Features

- Prints task names, states, priorities, stack margins, and numbers.
- Allocates an array sized for a system task-state snapshot.
- Reads detailed information and the stack high-water mark of one task.
- Releases the temporary FreeRTOS heap allocation after use.

## Hardware

USART1 on PA9 for TX and PA10 for RX at 115200 baud is the main observation interface. KEY1 and KEY2 use PA0 and PA1 and can confirm that the second task continues scanning input.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Reset the board to view the task list, system-state table, and detailed task 1 record. Task 1 then prints a status message every six seconds.

## Key Files

- `Drivers/BSP/freertos_test/`: Task list and state-query logic.
- `Users/FreeRTOSConfig.h`: Trace and formatting feature settings.
- `Drivers/SYSTEM/uart1/`: State table output.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
