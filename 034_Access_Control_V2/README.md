**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 Password Access Control — Refactored (V2)

## Overview

This project preserves initial password setup, verification, password changes, a three-failure alarm, and three-second automatic relocking while improving application naming and responsibility boundaries. The keypad task only produces input messages, and a password state machine handles setup, input, and modification modes. OLED, buzzer, and relay-lock operations run asynchronously through dedicated queues. A mutex protects W25Q128 access, while correct, wrong, and alarm conditions are passed through an event group to one monitor task.

## Features

- Uses explicit password input, modification, and setup states.
- Separates business logic from hardware operations with consistent messages.
- Opens the lock on success and starts a three-second one-shot relock timer.
- Produces a long alarm after three failures and short feedback on success.

## Hardware

The 4×4 keypad uses PB0–PB2, PB10, and PB11–PB14. OLED SCL/SDA use PB8/PB9, the buzzer PB5, and the lock relay PB6. W25Q128 SPI signals use PA4–PA7.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. On first use, enter a password and press `#` to save it. Later, enter the password and press `#` to unlock. Press `*`, verify the old password, and enter a new one to modify it.

## Key Files

- `Drivers/BSP/app_sys_orchestrator/`: Refactored password state machine and tasks.
- `Drivers/BSP/password/`: Password data processing.
- `Drivers/BSP/oled/`, `Drivers/BSP/relay_lock/`, and `Drivers/BSP/w25q128/`: UI, lock, and storage.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
