**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Dynamic Task Creation

## Overview

This experiment demonstrates how FreeRTOS creates and manages tasks with dynamic memory allocation. The application calls `xTaskCreate()` to create LED1, LED2, and key-monitor tasks. FreeRTOS allocates each task stack and task control block from its heap. The two LED tasks blink at intervals of one second and 500 milliseconds. When KEY1 is pressed, the monitor task calls `vTaskDelete()` to remove the LED1 task and then clears its handle, preventing later access through a stale task reference.

## Features

- Enables dynamic FreeRTOS allocation and creates three tasks at the same priority.
- Uses `pdMS_TO_TICKS()` to convert milliseconds into scheduler ticks.
- Deletes the LED1 task at runtime through its task handle.
- Reports through USART1 when KEY1 is pressed after the task has already been deleted.

## Hardware

LED1 and LED2 are connected to PB8 and PB9 and are active low. KEY1 uses PA0 with an internal pull-up and is active when connected to ground. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, build it, and flash the board. Both LEDs initially blink. Press KEY1 to stop LED1 permanently while LED2 and the key-monitor task continue running.

## Key Files

- `Drivers/BSP/freertos_test/`: Dynamic task creation, execution, and deletion.
- `Drivers/BSP/key/`: Key initialization, scanning, and debouncing.
- `Users/FreeRTOSConfig.h`: Dynamic allocation and scheduler settings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
