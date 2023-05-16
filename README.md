# Raspberry Pi 3 Bare Metal OS

This project is a bare metal operating system for the Raspberry Pi 3, written in AArch64 assembly and C. It is designed to be a learning tool for understanding operating system fundamentals and Raspberry Pi hardware specifics. The operating system implements a basic shell, using the mailbox interface to communicate with the VideoCore GPU for output. 

## Features

- **Bare Metal Booting and Initial Setup:** The system boots up in AArch64 assembly, with a detailed boot sequence responsible for setting up the environment, including setting up the stack and transitioning from EL3 to EL2.

- **Exception Handling:** The boot code includes handlers for synchronous exceptions, IRQs, FIQs, and system errors. This enables the OS to react and handle different kinds of hardware and software exceptions.

- **Mailbox Interface for GPU Communication:** The mailbox interface is used to communicate with the VideoCore GPU. It allows the OS to set display properties and handle output functionalities.

- **Memory Management:** The OS includes basic memory management features. The `bss` section of memory, which is used for uninitialized global variables, is zeroed out during boot. This is a common practice in OS development to avoid undefined behavior.

- **Basic Shell:** The OS includes a basic shell which provides a simple command interface. This shell leverages the mailbox interface to send output to the GPU.

- **File System Support:** The OS includes support for the FAT file system (`fat.c`), a widely used file system for removable storage devices. This allows the OS to read and write files from SD cards, USB drives, etc.

- **Custom Linker Script:** A custom linker script is used to control the memory layout of the code, specifying where each section of the code should be loaded in memory.

- **QEMU Emulation Support:** The system can be tested locally using QEMU, a generic and open source machine emulator and virtualizer, which provides a convenient development and testing environment.
## Prerequisites

To build and run this operating system, you will need:

- A Raspberry Pi 3
- A GCC cross-compiler for AArch64
- QEMU with support for the `raspi3` machine type

## Building

You can build the project using the provided Makefile:

```bash
make
```

This command generates a `kernel8.img` file that can be loaded by the Raspberry Pi bootloader.

## Running

To run the operating system on the Raspberry Pi:

1. Copy the `kernel8.img` file to a SD card.
2. Insert the SD card into your Raspberry Pi.
3. Power on the Raspberry Pi.

To run the operating system in QEMU you can use makefile:

```bash
make run
```

## Code Overview (In Progress)

- The `boot.S` file contains the assembly code for booting up the system and transitioning from EL3 to EL2.

- The `mBox.c` and `mBox.h` files implement the mailbox interface used to communicate with the GPU.

- The `printf` function and basic shell functionality are implemented in the `shell.c` and `shell.h` files. The shell provides a simple command interface, and uses the mailbox interface to send output to the GPU.

- The `linker.ld` script specifies the memory layout of the code.

## Contributing

Contributions to this project are welcome! Please submit a pull request or open an issue to discuss your changes.

## License

This project is released under the MIT license. 
MIT License

Copyright (c) [year] [fullname]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

