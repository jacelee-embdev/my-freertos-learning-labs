**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Task Notification as Event Group

## Overview

This experiment uses the `eSetBits` task-notification mode to model event flags. KEY1 sets bit 0 in the receiver's notification value, and KEY2 sets bit 1. The receiver obtains and clears each batch of notification bits with `xTaskNotifyWait()`, then accumulates the result in a local variable. Once both bit 0 and bit 1 have appeared, it reports the combined event and clears the accumulator. This is a lightweight approach when several events have one dedicated receiving task.

## Features

- Carries several event flags in one 32-bit notification value.
- Generates bit 0 and bit 1 events from KEY1 and KEY2.
- Accumulates events that arrive in separate notification batches.
- Runs combined processing after both required events are present.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press KEY1 and KEY2 in either order. After both event types have been received, the terminal reports their combined value and the next cycle begins.

## Key Files

- `Drivers/BSP/freertos_test/`: Event-bit send, receive, and accumulation.
- `Drivers/BSP/key/`: Two-key event source.
- `Users/FreeRTOSConfig.h`: Task notification settings.
