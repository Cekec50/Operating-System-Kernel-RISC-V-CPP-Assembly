# RISC-V Operating System Core in C++

## Overview

This project is a lightweight, non-distributable operating system kernel for the 64-bit RISC-V architecture, developed in C++. It provides fundamental OS features, including preemptive multitasking, thread management, synchronization primitives, and dynamic memory allocation. The kernel is designed to serve as a foundational layer for running user-level applications in a controlled, multi-threaded environment. This project was developed as a part of the Operating Systems 1 course at the University of Belgrade, School of Electrical Engineering.

## Tech Stack

*   **Core Language:** C++ (using C++11 standard)
*   **Low-Level Programming:** C and RISC-V Assembly Language
*   **Architecture:** RISC-V (RV64IMA)
*   **Build System:** GNU Make
*   **Emulator:** QEMU (`qemu-system-riscv64`)

## Features

*   **Preemptive Scheduler:** A time-slice-based scheduler that enables preemptive multitasking between threads.
*   **Thread Management:** A C and C++ API for creating, managing, and terminating threads (`TCB.hpp`).
*   **Synchronization Primitives:**
    *   **Semaphores:** C and C++ APIs for semaphore creation, waiting (blocking), and signaling (`SemaphoreManager.hpp`).
    *   **Mutexes:** Implemented via semaphores for mutual exclusion.
*   **System Calls:** A well-defined syscall interface for transitioning between user mode (`u-mode`) and supervisor mode (`s-mode`).
*   **Memory Management:** A buddy system memory allocator for dynamic allocation and deallocation of memory blocks (`MemoryAllocator.hpp`).
*   **Console I/O:** Basic functionality for printing characters and strings to the console.

## Getting Started

Follow these instructions to build and run the operating system kernel on your local machine using the QEMU emulator.

### Prerequisites

You must have a RISC-V cross-compilation toolchain and QEMU installed.

*   **RISC-V Toolchain:** A `riscv64-unknown-elf` or `riscv64-linux-gnu` toolchain (includes `gcc`, `g++`, `ld`, `as`, etc.).
*   **QEMU:** The `qemu-system-riscv64` emulator.
*   **Make:** GNU Make or a compatible alternative.

On Debian/Ubuntu-based systems, these can often be installed via:
```sh
sudo apt-get update
sudo apt-get install build-essential gcc-riscv64-unknown-elf qemu-system-misc
```

### Build

To compile the kernel, navigate to the root directory of the project and run the `make` command. This will compile all source files and link them into a kernel image.

```sh
make
```

This command creates the `kernel` executable file in the root directory and a `build/` directory for object files.

### Run in QEMU

To boot the compiled kernel in the QEMU emulator, run the following command:

```sh
make qemu
```

This will launch QEMU and load the `kernel` image. All console output from the kernel and its user programs will be displayed in your terminal.

## Project Structure

The codebase is organized into the following main directories:

```
.
├── h/                  # Header files for all C++ classes and C APIs.
├── src/                # Source files (.cpp, .c, .S) implementing the kernel features.
├── lib/                # Pre-compiled libraries for hardware access and console functions.
├── test/               # User-space test programs and examples.
├── Makefile            # Build script for compiling and running the project.
└── kernel.ld           # Linker script for memory layout.
```

## Usage

The primary entry point for user code is `test/userMain.cpp`. You can modify this file to write your own multi-threaded applications using the provided C/C++ APIs for thread creation, semaphores, and console output.

For example, to create a new thread, you would use the `thread_create` function.

After modifying `test/userMain.cpp` or other test files, simply re-run the `make` and `make qemu` commands to see your changes in action. The tests in the `test/` directory serve as excellent examples of how to use the different APIs.
