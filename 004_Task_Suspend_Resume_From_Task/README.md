**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Task Suspend and Resume

## Overview

This experiment demonstrates how one FreeRTOS task can suspend and resume another task from normal task context. The application creates LED1, LED2, and key-monitor tasks at the same priority. LED1 changes state every second and LED2 every 500 milliseconds. Pressing KEY1 makes the monitor task call `vTaskSuspend()` with the LED1 task handle. Pressing KEY2 calls `vTaskResume()` and allows LED1 to continue from the point where it was suspended. LED2 keeps running throughout the test, showing that suspending one task does not stop the scheduler.

## Features

- Controls a specific task through its stored task handle.
- Demonstrates `vTaskSuspend()` and `vTaskResume()` in task context.
- Provides independent LED activity for observing scheduler behavior.
- Prints suspend and resume events through USART1.

## Hardware

LED1 and LED2 use PB8 and PB9 and are active low. KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and become active when grounded. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, then build and flash it. Press KEY1 to freeze LED1 in its current state. Press KEY2 to resume LED1 while LED2 continues blinking normally.

## Key Files

- `Drivers/BSP/freertos_test/`: Task creation, suspension, and resumption.
- `Drivers/BSP/key/`: Two-key scanning and debouncing.
- `Users/main.c`: Peripheral initialization and scheduler startup.
