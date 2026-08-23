**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Message Queue

## Overview

This experiment demonstrates the basic use of a FreeRTOS queue for passing small messages between tasks. The system creates a three-entry queue whose items are one byte each, along with LED indicator, queue receiver, and key sender tasks. When KEY1 or KEY2 is pressed, the sender copies the corresponding key value into the queue. The receiver waits with `portMAX_DELAY` and prints each received value through USART1. While the queue is empty, the receiver remains blocked instead of consuming CPU time in a polling loop.

## Features

- Creates a byte-message queue with `xQueueCreate()`.
- Copies key values between tasks with `xQueueSend()`.
- Waits efficiently for data with blocking `xQueueReceive()`.
- Reports queue creation, send, and receive results for observation.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups and are active when grounded. LED1 uses PB8. USART1 uses PA9/PA10 at 115200 baud.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, build and flash it, and connect a serial terminal. Press either key. The terminal reports a successful queue send followed by the value received by the consumer task.

## Key Files

- `Drivers/BSP/freertos_test/`: Queue creation, producer, and consumer tasks.
- `Drivers/BSP/key/`: Key input and debouncing.
- `Drivers/SYSTEM/uart1/`: Debug output.
