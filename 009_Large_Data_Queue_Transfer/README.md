**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F103C8T6 FreeRTOS Large Data Queue Transfer

## Overview

This experiment demonstrates a common pointer-based approach for transferring large data through a FreeRTOS queue. The application prepares a large static character buffer, but defines each queue item as a `char *`. The key task therefore places only the buffer address into the queue instead of copying the complete text. The receiver blocks until a pointer arrives, then prints the referenced string through USART1. This approach greatly reduces queue memory usage and copy overhead, but the application must keep the referenced buffer valid and protect it from unsafe concurrent modification.

## Features

- Creates a three-entry queue whose items are character pointers.
- Sends the address of a static message buffer after either key is pressed.
- Receives the pointer and prints the complete referenced text.
- Illustrates the difference between small-item copying and pointer-based large-data transfer.

## Hardware

KEY1 and KEY2 use PA0 and PA1 with internal pull-ups. LED1 uses PB8. USART1 uses PA9/PA10 at 115200 baud for the long text output.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. Press either key to make the sender and receiver print the same large message.

## Key Files

- `Drivers/BSP/freertos_test/freertos_test.h`: Pointer queue-item type.
- `Drivers/BSP/freertos_test/freertos_test.c`: Message buffer and queue tasks.
- `Drivers/SYSTEM/uart1/`: Long debug-message output.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
