**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Relative and Absolute Delays

## Overview

This experiment compares the effect of the relative `vTaskDelay()` API with the absolute `xTaskDelayUntil()` API in periodic tasks. Each task first toggles an LED and performs a 20-millisecond busy wait to simulate application work. The LED1 task then delays for another 500 milliseconds, producing an actual period of about 520 milliseconds. The LED2 task delays relative to its previous wake time, keeping the complete cycle close to 500 milliseconds and preventing execution time from accumulating as timing drift.

## Features

- Shows how task execution time extends a relative-delay period.
- Establishes an absolute timing reference with `xTaskGetTickCount()`.
- Uses `xTaskDelayUntil()` for stable periodic wakeups.
- Prints an overload warning if the task misses its timing deadline.

## Hardware

LED1 on PB8 represents relative delay and LED2 on PB9 represents absolute delay. Both LEDs are active low. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. Observe both LEDs over an extended period. LED1 gradually shifts relative to LED2 because its processing time is added to every cycle. The serial terminal reports any detected overload condition.

## Key Files

- `Drivers/BSP/freertos_test/`: Tasks implementing both delay methods.
- `Drivers/BSP/led/`: Dual-LED driver.
- `Drivers/SYSTEM/delay/`: Busy-wait function used to simulate processing.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
