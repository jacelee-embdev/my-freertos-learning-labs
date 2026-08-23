**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Event Group State Query

## Overview

This experiment demonstrates how to set event bits and read the current FreeRTOS event-group state without modifying it. KEY1 sets bit 0 and KEY2 sets bit 1. A monitor task calls `xEventGroupGetBits()` once per second and reports each key event currently represented in the returned value. The query does not clear any flags, so a bit remains visible in every later monitoring cycle after it has been set.

## Features

- Sets independent flags with `xEventGroupSetBits()`.
- Obtains a current state snapshot with `xEventGroupGetBits()`.
- Tests bit 0 and bit 1 separately so several events can be reported together.
- Shows that reading the event group does not clear its bits.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and become active when grounded. USART1 uses PA9 for TX and PA10 for RX at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 or KEY2 and wait up to about one second. The matching flag remains in subsequent reports. After both keys have been pressed, both events continue to appear.

## Key Files

- `Drivers/BSP/freertos_test/`: Event-bit setting and state query.
- `Drivers/BSP/key/`: Two-key event input.
- `Users/FreeRTOSConfig.h`: Event-group settings.
