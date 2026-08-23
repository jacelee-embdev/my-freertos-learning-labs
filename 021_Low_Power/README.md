**English** | [简体中文](README_zh.md)

# STM32F103C8T6 FreeRTOS Tickless Low Power

## Overview

This experiment enables FreeRTOS Tickless Idle and places the STM32F103 into deep sleep while the application has a long blocked interval. An LED task changes state every five seconds. During the remaining time, pre-sleep and post-sleep hooks prepare STOP mode. Before sleeping, the application programs an RTC alarm driven by the 32.768 kHz LSE and disables several GPIO clocks. When the RTC interrupt wakes the MCU, the code restores the 72 MHz system clock and re-enables the peripheral clocks.

## Features

- Enables `configUSE_TICKLESS_IDLE` and sleep processing hooks.
- Programs the RTC alarm from the expected idle tick count.
- Disables GPIOB, GPIOC, and GPIOD clocks before sleep.
- Restores the PLL clock tree and LED peripherals after STOP wakeup.

## Hardware

LED1 and LED2 use PB8 and PB9. USART1 uses PA9/PA10. The RTC requires an external 32.768 kHz LSE crystal; wake timing may fail if the board does not have a working LSE source.

## Quick Start

Open `Projects/MDK-ARM/STM32F103.uvprojx`, build and flash it, then connect a serial terminal. LED1 changes about every five seconds and LED2 toggles in the RTC alarm interrupt. For power measurements, place an ammeter in the supply path and compare active and sleep current.

## Key Files

- `Drivers/BSP/freertos_test/`: Tickless pre-sleep and post-sleep handling.
- `Drivers/BSP/rtc/`: LSE, RTC alarm, and wake interrupt.
- `Users/FreeRTOSConfig.h`: Low-power configuration.
