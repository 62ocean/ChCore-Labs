# ChCore

This is the repository of ChCore labs in SE315, 2022 Spring.

It includes a series of labs in which I completed some components or code snippets of **the microkernel operating system ChCore**. These labs provided me with a deep understanding of the operating system's principles and how it functions.

## Contents
- [ChCore](#chcore)
  - [Contents](#contents)
  - [Labs](#labs)
    - [Lab1: Machine Bootup](#lab1-machine-bootup)
    - [Lab2: Memory Management](#lab2-memory-management)
    - [Lab3: Processes, Threads, and Exception Handling](#lab3-processes-threads-and-exception-handling)
    - [Lab4: Multi-Core, Multi-Process, Scheduling, and IPC](#lab4-multi-core-multi-process-scheduling-and-ipc)
    - [Lab5: File System and SHELL](#lab5-file-system-and-shell)
    - [Lab6: Device Drivers and Persistence](#lab6-device-drivers-and-persistence)
  - [Build](#build)
  - [Emulate](#emulate)
  - [Debug with GBD](#debug-with-gbd)
  - [Grade](#grade)
  - [Other](#other)


## Labs

### Lab1: Machine Bootup

This lab serves as the first lab for the ChCore Operating System course. It is divided into two parts: the first part introduces **the fundamental knowledge** required, and the second part familiarizes students with **the startup process of the ChCore kernel**.

### Lab2: Memory Management

The main purpose of this lab is to familiarize students with the initialization of memory during the kernel startup process and the management of physical memory and page tables after the kernel starts. It consists of three parts: **kernel startup page table, physical memory management, and page table management**.

### Lab3: Processes, Threads, and Exception Handling

User processes are an abstraction provided by the operating system for programs running in user mode. In Lab 1 and Lab 2, we have completed the kernel startup, physical memory management, and implemented a page table for user processes to use. Now, we will step by step support the execution of user-mode programs.

This lab consists of three parts:

1. Using the corresponding data structures, support the creation, startup, and management of user processes and threads.
2. Enhance the exception handling process and add necessary support for exception handling in the system.
3. Handle specific system calls correctly to ensure the normal execution and termination of user programs.

### Lab4: Multi-Core, Multi-Process, Scheduling, and IPC

In this lab, ChCore will support booting on multi-core processors (Part 1); implement a multi-core scheduler to schedule and execute multiple threads (Part 2); implement the first user-space system service: a process manager and support the `spawn` system call to launch new processes (Part 3); finally, implement kernel semaphores (Part 4).

### Lab5: File System and SHELL

In a microkernel architecture, the file system runs as a user-space system service. 

The first part of this experiment will implement **a user-space memory file system based on index nodes (inodes)**: a temporary file system (tmpfs). The second part will implement **a shell program** that provides a way for users to interact with the operating system, including simple commands related to files and directories. The third part will implement **a simple virtual file system** (VFS) in a microkernel architecture, capable of managing different file systems and providing a unified file operation interface for applications.

### Lab6: Device Drivers and Persistence

Drivers are a critical component of the operating system. They enable the computer to receive external input and output information to the outside world. This makes user interaction, data persistence, network communication, and more possible. 

In this experiment, we will implement an SD card device driver on ChCore (Part 1), allowing ChCore to access persistent data in the SD card. On top of this, we will add support for common block device file systems (Part 2).

## Build

- `make` or `make build`: Build ChCore
- `make clean`: Clean ChCore

## Emulate

- `make qemu`: Start a QEMU instance to run ChCore

## Debug with GBD

- `make qemu-gdb`: Start a QEMU instance with GDB server
- `make gdb`: Start a GDB (gdb-multiarch) client

## Grade

- `make grade`: Show your grade of labs in the current branch

## Other

- Press `Ctrl+a x` to quit QEMU
- Press `Ctrl+d` to quit GDB  


