**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Software Timers

## Overview

This experiment demonstrates the creation and control of one-shot and periodic FreeRTOS software timers. Timer 1 has a period of 5000 scheduler ticks and is one-shot; with the current 1 kHz tick configuration, it expires after about five seconds. Timer 2 has a 1000-tick, one-second period and automatically reloads. KEY1 and KEY2 toggle the start or stop state of the corresponding timer. A shared callback reads each timer's ID to identify the source and prints its accumulated callback count.

## Features

- Creates one one-shot timer and one auto-reload timer.
- Distinguishes timer instances by ID in a shared callback.
- Checks current state with `xTimerIsTimerActive()`.
- Starts and stops each timer dynamically through a dedicated key.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9 for TX and PA10 for RX at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. KEY1 controls the five-second one-shot timer and KEY2 controls the one-second periodic timer. The terminal displays state changes and callback counts.

## Key Files

- `Drivers/BSP/freertos_test/`: Timer creation, control, and callback.
- `Drivers/BSP/key/`: Timer start/stop input.
- `Users/FreeRTOSConfig.h`: Software timer service configuration.
