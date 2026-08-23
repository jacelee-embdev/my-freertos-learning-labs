**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Static Task Creation

## Overview

This experiment demonstrates how to create application tasks without asking the FreeRTOS heap to allocate their memory. The LED1, LED2, and key-monitor tasks are created with `xTaskCreateStatic()`. Each task receives a predefined `StackType_t` stack array and a `StaticTask_t` task control block. Explicit ownership of these buffers makes RAM usage and task lifetime more predictable, which is useful in embedded systems with strict memory-layout or reliability requirements.

## Features

- Creates three application tasks from static stacks and task control blocks.
- Implements `vApplicationGetIdleTaskMemory()` to supply memory for the kernel idle task.
- Implements `vApplicationGetTimerTaskMemory()` for the software timer service task.
- Blinks two LEDs at different rates and deletes the LED1 task when KEY1 is pressed.

## Hardware

LED1 and LED2 are connected to PB8 and PB9 and are active low. KEY1 uses PA0 with an internal pull-up and is active when grounded. USART1 uses PA9/PA10 at 115200 baud to print task-deletion messages.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, build the project, and flash the board. After reset, both LEDs blink. Press KEY1 to delete the LED1 task; the remaining tasks continue to run normally.

## Key Files

- `Drivers/BSP/freertos_test/`: Static tasks, stacks, TCBs, and kernel memory callbacks.
- `Users/FreeRTOSConfig.h`: Static allocation and software timer settings.
- `Users/main.c`: Peripheral initialization and scheduler startup.
