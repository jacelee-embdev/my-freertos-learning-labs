**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Mutex Holder Query

## Overview

This experiment adds mutex-owner inspection to a priority inheritance scenario. The low-priority task first takes the mutex and performs a long simulated operation. Before the high-priority task requests the resource, it calls `xSemaphoreGetMutexHolder()` to obtain the current owner's task handle and then prints the owner's name with `pcTaskGetName()`. The high-priority task subsequently blocks on the mutex and triggers the FreeRTOS priority inheritance mechanism.

## Features

- Protects a simulated shared resource with a mutex.
- Checks whether the mutex is currently owned by a task.
- Obtains and prints the owner name from its task handle.
- Observes priority inheritance with low-, medium-, and high-priority tasks.

## Hardware

No key input is required. Task activity is displayed through USART1 on PA9 for TX and PA10 for RX at 115200 baud. Join the serial adapter and board grounds.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. After reset, the High task reports `task_low` as the current mutex owner before the log continues through resource handoff and priority inheritance.

## Key Files

- `Drivers/BSP/freertos_test/`: Mutex contention and owner query.
- `Drivers/SYSTEM/delay/`: Simulated task load.
- `Drivers/SYSTEM/uart1/`: Owner and scheduling logs.
