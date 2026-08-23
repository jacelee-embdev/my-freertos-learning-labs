**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Protected Debug Printing

## Overview

This experiment provides a simple solution for interleaved characters, broken messages, or garbled output when several FreeRTOS tasks call `printf()` concurrently. The `debug_printf()` macro enters a critical section before formatting and transmitting a message, then leaves the critical section after the output is complete. This prevents another task or interrupt from splitting a debug line. Three tasks print startup messages while also running the dual-LED demonstration and the KEY1-controlled deletion of the LED1 task.

## Features

- Protects debug output with `taskENTER_CRITICAL()` and `taskEXIT_CRITICAL()`.
- Accepts the same variable argument syntax as standard `printf()`.
- Demonstrates protected output from multiple equal-priority tasks.
- Deletes the LED1 task with KEY1 and reports the result.

## Hardware

USART1 uses PA9 for TX and PA10 for RX at 115200 baud. LED1 and LED2 use PB8 and PB9. KEY1 uses PA0 and is active when grounded.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, then build and flash it. Connect a serial terminal and reset the board to view complete task startup messages. Because the critical section temporarily blocks scheduling and interrupts, this method should only be used for short, low-frequency debug output.

## Key Files

- `Drivers/SYSTEM/uart1/uart1.h`: Critical-section `debug_printf()` macro.
- `Drivers/BSP/freertos_test/`: Concurrent printing and task test logic.
- `Drivers/SYSTEM/uart1/`: USART1 driver and `printf()` redirection.
