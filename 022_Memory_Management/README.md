**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Memory Management

## Overview

This experiment demonstrates allocation, release, and free-space queries for the FreeRTOS heap. The project uses `heap_4.c`. KEY1 calls `pvPortMalloc(30)` to allocate a 30-byte buffer, while KEY2 calls `vPortFree()` and clears the stored pointer. A second task calls `xPortGetFreeHeapSize()` every two seconds, allowing the serial terminal to show how task creation and runtime allocations affect the remaining FreeRTOS heap.

## Features

- Allocates and releases a 30-byte buffer through FreeRTOS memory APIs.
- Clears the pointer after release to prevent double-free and stale access.
- Reports the current free heap space periodically.
- Prints allocation and release success or failure.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 to allocate and KEY2 to release the buffer. Do not press KEY1 repeatedly before freeing; the stored pointer would be overwritten and the older allocation could no longer be released.

## Key Files

- `Drivers/BSP/freertos_test/`: Allocation, release, and monitoring tasks.
- `Middlewares/FreeRTOS/portable/MemMang/heap_4.c`: Heap implementation.
- `Users/FreeRTOSConfig.h`: FreeRTOS heap-size configuration.
