**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Resume Task from ISR

## Overview

This experiment demonstrates how to resume a suspended FreeRTOS task safely from an external interrupt service routine. The application runs LED1, LED2, and key-monitor tasks. Pressing KEY1 makes the monitor task suspend LED1 with `vTaskSuspend()`. KEY2 on PA1 is configured as a falling-edge EXTI1 interrupt. Its callback resumes LED1 with `xTaskResumeFromISR()`. When the returned value indicates that a context switch is required, `portYIELD_FROM_ISR()` requests the switch as the interrupt exits.

## Features

- Suspends the LED1 task from normal task context.
- Resumes the suspended task from the PA1 external interrupt.
- Uses the FreeRTOS FromISR API and ISR yield macro correctly.
- Keeps LED2 running as an independent scheduler indicator.

## Hardware

LED1 and LED2 use PB8 and PB9. KEY1 uses PA0 to suspend the task, while KEY2 uses PA1 to trigger the falling-edge interrupt. Both inputs use internal pull-ups and are active when grounded. USART1 uses PA9/PA10.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx` in Keil MDK, then build and flash it. Press KEY1 to suspend LED1, followed by KEY2 to trigger EXTI1 and resume the task. The resume event is also printed to the serial terminal.

## Key Files

- `Drivers/BSP/freertos_test/`: Task creation and suspension.
- `Drivers/BSP/exti/`: EXTI1 setup and ISR resume logic.
- `Users/main.c`: Peripheral initialization and scheduler entry.
