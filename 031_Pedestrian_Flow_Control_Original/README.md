**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Pedestrian Flow Control System — Original

## Overview

This integrated experiment counts pedestrian passages with an infrared sensor and controls a gate, LCD1602, LEDs, and buzzer. The system initially permits entry, while a counting semaphore represents five available passage slots. Every falling-edge infrared event on PB4 consumes one slot and updates the count on the LCD. After the fifth passage, the system enters a waiting state, closes the relay-controlled gate, flashes the warning LED rapidly, and activates the buzzer. A three-second one-shot timer then restores all slots and returns the system to the pass state.

## Features

- Represents five passage slots with a counting semaphore.
- Coordinates sensor and actuator tasks through a state queue.
- Serializes LCD updates through a display queue and mutex.
- Uses a timer callback and task notification to reset the system.

## Hardware

The infrared sensor uses PB4, the buzzer PB5, the gate relay PB6, and LED1/LED2 PB8/PB9. LCD1602 D0–D7 use PA0–PA7; RS, RW, and EN use PB1, PB2, and PB10.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. Trigger the infrared sensor repeatedly. The LCD advances from `PASS...00/05` to the fifth passage, then the gate closes and the alarm starts. Normal operation returns after about three seconds.

## Key Files

- `Drivers/BSP/freertos_test/`: Original application tasks and RTOS objects.
- `Drivers/BSP/exti/`: Infrared detection interrupt.
- `Drivers/BSP/lcd1602/` and `Drivers/BSP/relay_gate/`: Display and gate drivers.
