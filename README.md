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

## Complete Experiment Index

The tables below list all 37 top-level subprojects in the repository. Click a project name to open its directory. Except for the FreeRTOS source directory, every experiment contains a Chinese `README_zh.md` and an English `README.md` with more detailed information.

### 000–007: Source, Porting, and Task Fundamentals

| No. | Project | Description |
| ---: | --- | --- |
| 000 | [FreeRTOS_LTS_Source_v202406_04](000_FreeRTOS_LTS_Source_v202406_04/) | Preserves a FreeRTOS LTS 202406.04 source snapshot for experiment builds and kernel source reference. |
| 001 | [FreeRTOS_Porting](001_FreeRTOS_Porting/) | Ports FreeRTOS to the STM32F103C8T6 and verifies scheduling with two LED tasks running at different periods. |
| 002 | [Dynamic_Task_Creation](002_Dynamic_Task_Creation/) | Creates three tasks dynamically and deletes the LED1 task by handle when a button is pressed. |
| 003 | [Static_Task_Creation](003_Static_Task_Creation/) | Creates tasks with static stacks and TCBs and supplies static memory for the kernel idle and timer tasks. |
| 004 | [Task_Suspend_Resume_From_Task](004_Task_Suspend_Resume_From_Task/) | Suspends and resumes a selected LED task from normal task context using two buttons. |
| 005 | [Task_Suspend_Resume_From_ISR](005_Task_Suspend_Resume_From_ISR/) | Suspends an LED task from a task, then safely resumes it from an external interrupt with an optional context switch. |
| 006 | [Printf_Garbled_Output_Fix](006_Printf_Garbled_Output_Fix/) | Protects multitask `printf()` calls with a critical section to prevent interleaved, broken, or garbled serial output. |
| 007 | [Relative_Vs_Absolute_Delay](007_Relative_Vs_Absolute_Delay/) | Compares relative and absolute delays to show how task execution time affects periodic timing drift. |

### 008–015: Queues, Semaphores, and Event Groups

| No. | Project | Description |
| ---: | --- | --- |
| 008 | [Message_Queue](008_Message_Queue/) | Passes small button messages through a byte queue from a sender task to a blocking receiver task. |
| 009 | [Large_Data_Queue_Transfer](009_Large_Data_Queue_Transfer/) | Passes a static buffer pointer through a queue to demonstrate how large data copies can be avoided. |
| 010 | [Binary_Semaphore](010_Binary_Semaphore/) | Uses two buttons to demonstrate binary semaphore give/take operations and full or empty failure cases. |
| 011 | [Counting_Semaphore](011_Counting_Semaphore/) | Increments and decrements a counting semaphore with buttons while displaying resource counts from 0 to 5. |
| 012 | [Priority_Inversion](012_Priority_Inversion/) | Reproduces priority inversion with low-, medium-, and high-priority tasks sharing a binary semaphore. |
| 013 | [Mutex_Semaphore](013_Mutex_Semaphore/) | Protects the shared resource with a mutex to demonstrate how priority inheritance mitigates priority inversion. |
| 014 | [Queue_Set](014_Queue_Set/) | Uses a queue set so one task can wait on both a data queue and a binary semaphore. |
| 015 | [Event_Group](015_Event_Group/) | Waits for two button event bits together and automatically clears them after the condition is met. |

### 016–020: Task Notifications and Software Timers

| No. | Project | Description |
| ---: | --- | --- |
| 016 | [Task_Notification_As_Binary_Semaphore](016_Task_Notification_As_Binary_Semaphore/) | Clears the task notification value on receipt so multiple pending notifications merge into one binary event. |
| 017 | [Task_Notification_As_Counting_Semaphore](017_Task_Notification_As_Counting_Semaphore/) | Increments and decrements a notification value one at a time to model a counting semaphore with queued events. |
| 018 | [Task_Notification_As_Mailbox](018_Task_Notification_As_Mailbox/) | Uses the 32-bit task notification value as an overwriting single-slot mailbox that retains only the latest value. |
| 019 | [Task_Notification_As_Event_Group](019_Task_Notification_As_Event_Group/) | Sets notification bits with `eSetBits` to model a lightweight event group for one receiving task. |
| 020 | [Software_Timer](020_Software_Timer/) | Creates one-shot and auto-reload software timers, controls them with buttons, and counts callback executions. |

### 021–030: Low Power, Memory, and Utility APIs

| No. | Project | Description |
| ---: | --- | --- |
| 021 | [Low_Power](021_Low_Power/) | Enables Tickless Idle, enters STOP mode during idle time, and wakes through an RTC alarm. |
| 022 | [Memory_Management](022_Memory_Management/) | Allocates and frees memory with `heap_4` while periodically reporting the remaining FreeRTOS heap. |
| 023 | [Task_Utility_APIs_1](023_Task_Utility_APIs_1/) | Demonstrates task priority access, name-based handle lookup, and suspension of selected and current tasks. |
| 024 | [Task_Utility_APIs_2](024_Task_Utility_APIs_2/) | Queries task counts, states, run-time counters, and stack high-water marks and prints the system task list. |
| 025 | [Queue_Utility_APIs](025_Queue_Utility_APIs/) | Queries free queue slots and pending message counts and demonstrates resetting a full queue. |
| 026 | [Semaphore_Utility_APIs](026_Semaphore_Utility_APIs/) | Identifies the current mutex holder and observes priority inheritance with three task priorities. |
| 027 | [Timer_Utility_APIs](027_Timer_Utility_APIs/) | Queries software timer periods, IDs, active states, and next expiration times. |
| 028 | [Event_Group_Utility_APIs_1](028_Event_Group_Utility_APIs_1/) | Reads event group state without modifying it, showing that set bits remain after inspection. |
| 029 | [Event_Group_Utility_APIs_2](029_Event_Group_Utility_APIs_2/) | Builds a multitask synchronization barrier for three logical participants with `xEventGroupSync()`. |
| 030 | [Memory_Utility_APIs](030_Memory_Utility_APIs/) | Reports heap statistics including free space, largest free block, and cumulative allocation and free counts. |

### 031–033: Integrated Applications and Refactoring

| No. | Project | Description |
| ---: | --- | --- |
| 031 | [Pedestrian_Flow_Control_Original](031_Pedestrian_Flow_Control_Original/) | Combines infrared counting, a gate, LCD, LEDs, and a buzzer for five-person flow control with automatic recovery. |
| 031 | [Pedestrian_Flow_Control_Refactored](031_Pedestrian_Flow_Control_Refactored/) | Preserves the five-person limit while restructuring the application around a state machine, actuator, and display task. |
| 032 | [Access_Control_Original](032_Access_Control_Original/) | Implements keypad access control with W25Q128 password storage, failed-attempt alarms, and automatic relocking. |
| 032 | [Access_Control_Refactored](032_Access_Control_Refactored/) | Preserves the access-control features while clarifying the password state machine, messages, and software/hardware roles. |
| 033 | [Smart_Desk_Lamp_Original](033_Smart_Desk_Lamp_Original/) | Integrates presence sensing, automatic dimming, distance and sitting-time alarms, OLED output, and Bluetooth control. |
| 033 | [Smart_Desk_Lamp_Refactored](033_Smart_Desk_Lamp_Refactored/) | Preserves all three operating modes while restructuring shared state, thresholds, initialization, and task coordination. |

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

## License

This repository is released under the [MIT License](LICENSE). Third-party source code remains subject to its own licenses and notices.
