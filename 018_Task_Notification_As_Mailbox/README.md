**English** | [简体中文](README_zh.md)

# STM32F103C8T6 Task Notification as Mailbox

## Overview

This experiment uses a task's 32-bit notification value as a single-slot mailbox. The sender maintains an incrementing value and writes it to the receiver whenever KEY1 is pressed by calling `xTaskNotify()` with `eSetValueWithOverwrite`. The receiver blocks in `xTaskNotifyWait()` and delays for one second after processing. Because overwrite mode is enabled, a new send replaces an older value that has not yet been read. This models communication where only the latest state matters.

## Features

- Transfers one 32-bit value directly through a task notification.
- Continuously updates the mailbox with overwrite semantics.
- Blocks the receiver and clears notification bits after reading.
- Demonstrates how rapid sends replace older unread values.

## Hardware

KEY1 uses PA0 with an internal pull-up and becomes active when grounded. USART1 uses PA9 for TX and PA10 for RX at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Slow KEY1 presses produce consecutive values. Pressing rapidly may make the receiver skip intermediate numbers and read only the newest mailbox value available when it runs.

## Key Files

- `Drivers/BSP/freertos_test/`: Mailbox send, overwrite, and receive logic.
- `Drivers/BSP/key/`: KEY1 input.
- `Users/FreeRTOSConfig.h`: Task notification settings.
