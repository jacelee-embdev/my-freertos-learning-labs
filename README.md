**English** | [简体中文](README_zh.md)

# My FreeRTOS Learning Labs

## About This Repository

This is a personal collection of FreeRTOS learning experiments for the STM32F103C8T6. Each subproject focuses on a relatively independent topic and preserves the code used to study, test, and review that topic. The repository begins with FreeRTOS porting and task management, then covers queues, semaphores, mutexes, event groups, task notifications, software timers, low-power operation, memory management, and useful FreeRTOS APIs. The final projects combine several concepts in small applications and compare original implementations with refactored versions.

This repository is primarily a learning archive and code notebook. It is not an official tutorial, a general-purpose component library, or production-ready firmware. Its main purpose is to keep useful experimental code easy to find and reproduce while using Git history and a GitHub remote copy to reduce the risk of losing local files.

## Goals

- Keep FreeRTOS experiments and debugging examples in one organized repository.
- Preserve small, focused examples as references for future embedded projects.
- Track changes with Git so previous versions can be compared, restored, or reviewed.
- Maintain a remote copy on GitHub for access and synchronization across devices.
- Document each experiment in both Chinese and English.

## Repository Overview

| Range | Topics |
| --- | --- |
| `000` | FreeRTOS LTS source snapshot used by the experiments and kept for reference |
| `001`–`007` | Porting, dynamic and static tasks, task suspend/resume, and delay methods |
| `008`–`015` | Queues, semaphores, mutexes, queue sets, and event groups |
| `016`–`020` | Task notification patterns and software timers |
| `021`–`030` | Low-power operation, memory management, and utility APIs |
| `031`–`033` | Pedestrian flow control, access control, and smart desk lamp applications, each with original and refactored versions |

Subproject directories follow the `number_English_Name` naming convention. Except for the FreeRTOS source directory, every experiment contains a Chinese `README_zh.md` and an English `README.md` with its purpose, key concepts, and expected behavior.

## Development Environment

- MCU: STM32F103C8T6
- RTOS: FreeRTOS
- Peripheral library: STM32 HAL
- IDE and toolchain: Keil MDK-ARM

Peripherals, serial output, and hardware connections vary between experiments. Check the README and source configuration inside the selected subproject before running it on a board.

## Getting Started

1. Clone or download this repository.
2. Enter the directory of the experiment you want to study.
3. Open `Projects/MDK-ARM/STM32F103.uvprojx` with Keil MDK-ARM.
4. Review the clock, pin, and debugger settings for your board, then build and flash the firmware.
5. Observe the result through serial output, LEDs, or the peripherals used by that experiment.

## Backup and Security

Git history and a GitHub remote copy improve traceability and reduce the risk of accidental code loss, but they are not a complete backup strategy. Keep additional copies of important work when necessary. If the repository is public, anyone can view its files, so never commit passwords, access tokens, private keys, certificates, device credentials, or other sensitive information.

## License

This repository is released under the [MIT License](LICENSE). Third-party source code remains subject to its own licenses and notices.
