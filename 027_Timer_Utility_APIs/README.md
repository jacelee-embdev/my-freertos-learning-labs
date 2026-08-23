**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Timer Utility APIs

## Overview

This experiment extends one-shot and periodic software timers with queries for timer period, ID, and expiry time. Timer 1 is a five-second one-shot timer with ID 1, while timer 2 is a one-second auto-reload timer with ID 2. KEY1 and KEY2 toggle the corresponding timers. Once per second, a monitor task reads timer 1's period and ID, then subtracts the current scheduler tick from timer 2's expiry tick to report the remaining ticks until its next callback.

## Features

- Queries a software timer's configured period and user ID.
- Reads the next expiry tick and calculates remaining time.
- Checks active state and toggles timers through keys.
- Counts callback executions for both timers in one shared function.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Use KEY1 for the five-second one-shot timer and KEY2 for the one-second periodic timer. While active, the log shows period, ID, remaining ticks, and callback counts.

## Key Files

- `Drivers/BSP/freertos_test/`: Timer queries, monitor, and callbacks.
- `Drivers/BSP/key/`: Timer control input.
- `Users/FreeRTOSConfig.h`: Timer service settings.
