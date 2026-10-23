**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Heap Statistics APIs

## Overview

This experiment adds FreeRTOS heap statistics to the basic allocation and release test. The project uses `heap_4.c`. KEY1 allocates a 30-byte buffer and KEY2 releases the current buffer. Every two seconds, a monitor task calls `vPortGetHeapStats()` and prints the available space, largest contiguous free block, number of successful allocations, and number of successful frees. It also calls `xPortGetFreeHeapSize()` to report the total free heap, providing a simple view of heap usage and fragmentation.

## Features

- Obtains detailed heap status through `HeapStats_t`.
- Reports free space and the largest allocatable free block.
- Counts successful allocation and release operations.
- Observes statistics changing in response to key actions.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Alternate KEY1 and KEY2 while watching the statistics. Do not allocate repeatedly before freeing, because the previous pointer would be lost and its block would leak.

## Key Files

- `Drivers/BSP/freertos_test/`: Heap operations and statistics monitor.
- `Middlewares/FreeRTOS/portable/MemMang/heap_4.c`: Heap implementation.
- `Users/FreeRTOSConfig.h`: Heap size and memory settings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
