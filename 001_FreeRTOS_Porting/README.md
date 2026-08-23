**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Porting Experiment

## Overview

This experiment ports FreeRTOS to the STM32F103C8T6 and verifies that the kernel starts and schedules tasks correctly. The MCU runs at 72 MHz and the FreeRTOS tick rate is configured to 1 kHz. After the scheduler starts, two independent tasks blink LEDs at different rates: LED1 stays on and off for one second each, while LED2 changes state every 500 milliseconds. The simultaneous patterns provide a simple visual check of task switching and blocking delays.

## Features

- Provides the basic FreeRTOS kernel, port layer, and `FreeRTOSConfig.h` configuration.
- Creates two dynamically allocated tasks with `xTaskCreate()` at priorities 2 and 3.
- Uses `vTaskDelay()` to block tasks and release CPU time.
- Prints a startup message through USART1 for debugging.

## Hardware

LED1 is connected to PB8 and LED2 to PB9; both are active low. USART1 uses PA9 for TX and PA10 for RX at 115200 baud and may be connected to a serial terminal.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, build the project, and flash the board. After reset, both LEDs should blink continuously at different rates.

## Key Files

- `Users/FreeRTOSConfig.h`: FreeRTOS kernel configuration.
- `Drivers/BSP/freertos_test/`: Test task creation and behavior.
- `Users/main.c`: Hardware initialization and scheduler startup.
