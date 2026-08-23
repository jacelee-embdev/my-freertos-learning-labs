**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Smart Desk Lamp — Refactored

## Overview

This project preserves the smart lamp's three operating modes, automatic dimming, distance alarm, sitting-time reminder, and Bluetooth control while improving state names, threshold definitions, and initialization. The sensor task centrally updates presence, light, distance, and alarm flags. The device task drives the PWM lamp and buzzer from shared state, while display, key, and Bluetooth functions each run in dedicated tasks. A mutex protects the global status structure, initialization clears it consistently, and an event group carries alarm conditions.

## Features

- Selects a brightness level from presence and ambient light in automatic mode.
- Provides four key-controlled levels and Bluetooth commands `0`–`6` in remote mode.
- Alarms after five seconds below 5 cm or about ten seconds of timed use.
- Displays mode, presence, distance, and elapsed time on the OLED.

## Hardware

PWM lamp output uses PB0, light input PA1, presence input PB12, and HC-SR04 PA11/PA12. Four keys use PA4–PA7, the OLED PB6/PB7, the buzzer PB13, and Bluetooth USART2 PA2/PA3.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. KEY1 cycles automatic, key, and remote modes. KEY2 changes brightness, KEY3 controls timing, and KEY4 clears it. In remote mode, send single-character commands through Bluetooth.

## Key Files

- `Drivers/BSP/app_sys_orchestrator/`: Refactored shared state and task coordination.
- `Drivers/BSP/light_sensor/`, `Drivers/BSP/hcsr04/`, and `Drivers/BSP/ia_sensor/`: Sensor modules.
- `Drivers/BSP/led/`, `Drivers/BSP/bluetooth/`, and `Drivers/BSP/oled/`: Lighting, Bluetooth, and display drivers.
