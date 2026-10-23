**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Priority Inversion

## Overview

This experiment reproduces priority inversion with low-, medium-, and high-priority tasks sharing a normal binary semaphore. The low-priority task first takes the semaphore and begins a long simulated operation. The high-priority task later requests the same semaphore and becomes blocked. While it waits, the medium-priority task preempts the low-priority task, preventing the shared resource from being released promptly and indirectly delaying the high-priority task. Serial messages labeled Low, Middle, and High reveal the complete scheduling sequence.

## Features

- Creates three test tasks at priorities 1, 2, and 3.
- Protects a simulated shared resource with a binary semaphore.
- Uses busy waits to construct the resource-holding and preemption sequence.
- Shows that a binary semaphore does not provide priority inheritance.

## Hardware

The experiment is observed mainly through the serial terminal and requires no key input. USART1 uses PA9 for TX and PA10 for RX at 115200 baud. Connect all device grounds together.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Reset the board and follow the three labeled task logs. Compare the moment High starts waiting with the later point at which it finally acquires the semaphore.

## Key Files

- `Drivers/BSP/freertos_test/`: Priority inversion scenario and tasks.
- `Drivers/SYSTEM/delay/`: Simulated processing load.
- `Drivers/SYSTEM/uart1/`: Scheduling log output.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
