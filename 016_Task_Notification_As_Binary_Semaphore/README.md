**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Task Notification as Binary Semaphore

## Overview

This experiment uses a FreeRTOS task notification to provide one-way event synchronization similar to a binary semaphore. When the key task detects KEY1, it calls `xTaskNotifyGive()` for the receiver task. The receiver blocks in `ulTaskNotifyTake(pdTRUE, portMAX_DELAY)` until a notification arrives. The `pdTRUE` parameter clears the notification value after a successful receive, so several pending notifications are combined into one effective event and the behavior resembles a binary state.

## Features

- Uses a task's built-in notification value without creating a semaphore object.
- Sends a notification to a specific receiving task when KEY1 is pressed.
- Blocks the receiver efficiently while no event is available.
- Clears the notification value after receiving to model binary behavior.

## Hardware

KEY1 uses PA0 with an internal pull-up and is active when grounded. USART1 uses PA9 for TX and PA10 for RX at 115200 baud to display notification send and receive events.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Each KEY1 press first produces a send message and then wakes the receiving task, which reports that it acquired the event.

## Key Files

- `Drivers/BSP/freertos_test/`: Task notification sender and receiver.
- `Drivers/BSP/key/`: KEY1 scanning and debouncing.
- `Users/FreeRTOSConfig.h`: Task notification configuration.
