**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 Pedestrian Flow Control System — Refactored

## Overview

This project preserves the original five-person limit, gate control, and three-second automatic recovery while reorganizing the application into a sensor state machine, gate actuator, and dedicated display task. Each infrared event consumes one slot from a counting semaphore. When all slots are used, a state queue tells the actuator to close the PB6 gate, enable warning indications, and sound the buzzer. When the software timer expires, a task notification wakes the sensor task, which centrally restores the semaphore, system state, sensor flag, and LCD contents.

## Features

- Organizes the workflow with explicit allow-pass and locked-wait states.
- Separates business logic, actuators, and UI through two queues.
- Protects the non-reentrant LCD driver with a mutex.
- Fully resets pending sensor state and passage capacity after lockout.

## Hardware

The infrared sensor uses PB4, buzzer PB5, gate relay PB6, and LED1/LED2 PB8/PB9. LCD1602 D0–D7 use PA0–PA7, while RS/RW/EN use PB1/PB2/PB10.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. Trigger the infrared sensor five times and observe the LCD and gate. After about three seconds of lockout and alarm, stale triggers are cleared and `PASS...00/05` is restored.

## Key Files

- `Drivers/BSP/app_sys_orchestrator/`: Refactored state machine and task coordination.
- `Drivers/BSP/exti/`: Infrared event acquisition.
- `Drivers/BSP/lcd1602/` and `Drivers/BSP/relay_gate/`: Display and actuator drivers.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
