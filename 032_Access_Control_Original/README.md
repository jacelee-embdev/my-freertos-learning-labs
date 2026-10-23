**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 Password Access Control — Original

## Overview

This integrated experiment implements keypad-based access control with non-volatile password storage. At startup, the system reads the password from a W25Q128 device. If no password exists, it enters initial setup; otherwise it displays the normal password prompt. Number keys enter characters, `#` confirms an entry, and `*` begins password modification. A correct password produces a short beep and opens the relay lock. A three-second software timer closes it automatically. Three consecutive failures produce a long alarm. Dedicated tasks handle the OLED, buzzer, and lock.

## Features

- Stores and reads a password of up to ten bytes in W25Q128 flash.
- Routes keypad, display, buzzer, and lock commands through separate queues.
- Records correct, wrong, and repeated-failure alarm events in an event group.
- Protects SPI flash access with a mutex.

## Hardware

The 4×4 keypad uses PB0–PB2, PB10, and PB11–PB14. OLED SCL/SDA use PB8/PB9, the buzzer PB5, and the lock relay PB6. W25Q128 CS/SCK/MISO/MOSI use PA4–PA7.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. On first boot, enter a new password and press `#`. During normal use, enter the password and confirm with `#`. Press `*` to verify the old password and set a replacement.

## Key Files

- `Drivers/BSP/app_sys_orchestrator/`: Original access-control tasks and messaging.
- `Drivers/BSP/password/`: Password buffering, comparison, and storage.
- `Drivers/BSP/keyboard/` and `Drivers/BSP/w25q128/`: Input and non-volatile storage.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
