**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Smart Desk Lamp — Original

## Overview

This integrated experiment implements a smart desk lamp with presence sensing, ambient-light adjustment, ultrasonic distance measurement, sitting-time reminders, and Bluetooth control. The system provides automatic, key, and remote modes. In automatic mode, it selects high, medium, or low brightness from the measured light level while a person is present, and turns off when nobody is detected. The buzzer alarms if distance remains below 5 cm for five seconds or continuous use reaches about ten seconds. The OLED displays mode, presence, distance, and elapsed time.

## Features

- KEY1 changes mode; KEY2 cycles four brightness levels in key mode.
- KEY3 starts or pauses timing, while KEY4 stops and clears it.
- Remote mode accepts Bluetooth commands `0` through `6` for lighting and timing.
- Protects shared status with a mutex and manages two alarms with an event group.

## Hardware

PWM lamp output uses PB0, the light sensor PA1, presence sensor PB12, and HC-SR04 TRIG/ECHO PA11/PA12. Keys use PA4–PA7, OLED SCL/SDA PB6/PB7, buzzer PB13, and Bluetooth USART2 PA2/PA3.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build it, and flash the board. Select a mode with KEY1. Change presence and light inputs in automatic mode, adjust brightness with KEY2 in key mode, or send character commands over Bluetooth in remote mode.

## Key Files

- `Drivers/BSP/app_sys_orchestrator/`: Original sensing, display, and control tasks.
- `Drivers/BSP/light_sensor/`, `Drivers/BSP/hcsr04/`, and `Drivers/BSP/ia_sensor/`: Environment inputs.
- `Drivers/BSP/led/`, `Drivers/BSP/bluetooth/`, and `Drivers/BSP/oled/`: Lighting, remote control, and UI.
