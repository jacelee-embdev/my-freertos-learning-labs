**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Event Group

## Overview

This experiment uses a FreeRTOS event group to synchronize two independent events. KEY1 represents bit 0 and KEY2 represents bit 1. The key task sets the corresponding flag with `xEventGroupSetBits()`. The event handler blocks in `xEventGroupWaitBits()` with the wait-for-all option enabled, so it continues only after both flags have been set. Once the condition is satisfied, both bits are cleared automatically and the system begins another waiting cycle.

## Features

- Creates an event group with `xEventGroupCreate()`.
- Sets bit 0 and bit 1 independently through two keys.
- Synchronizes events with an all-bits logical condition.
- Clears the matched bits automatically and prints the returned value.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9 for TX and PA10 for RX at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 and KEY2 in either order. The event handler reports completion only after both keys have contributed their event bits.

## Key Files

- `Drivers/BSP/freertos_test/`: Event-bit producer and waiting tasks.
- `Drivers/BSP/key/`: Two-key input.
- `Users/FreeRTOSConfig.h`: Event-group kernel settings.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
