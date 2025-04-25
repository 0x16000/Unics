![Image](https://github.com/user-attachments/assets/0f6de833-16d6-4cea-8968-7ce4e6de3a50)

# Unics Operating System

Unics is a POSIX-compliant, Unix-like operating system inspired by BSD, designed to run on the i386 architecture. It aims to provide a lightweight, efficient, and educational platform for understanding low-level system concepts and kernel development. Unics includes a small `libc` for basic system functionality and uses the GRUB bootloader for boot management.

## Features

- **POSIX compliance** for compatibility with Unix-like software.
- BSD-inspired design and system utilities.
- Custom shell and basic command-line tools like `clear`, `echo`, `whoami`, and `cowsay`.
- A small, lightweight `libc` for essential system libraries.
- Simple, low-level kernel built for the i386 architecture.
- Uses the **GRUB bootloader** for system booting.
- Focus on simplicity, clarity, and performance.
- Bootloader with a `boot>` prompt for a unique experience.
- Minimalist design for learning and experimentation.

## Architecture

Unics is designed to run on 32-bit x86 hardware (i386). It is built with a focus on:
- Kernel-level development
- Bootstrapping a minimalistic operating system
- User-space utilities and applications
- System calls for basic interaction with hardware and the kernel

## License
BSD 3-Clause License

Copyright (c) 2025, 0x16000

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Getting Started

To build and run Unics:

1. Clone the repository:

   ```bash
   git clone https://github.com/0x16000/Unics.git
   cd Unics

2. Build the system:
   Make sure you have the necessary tools installed (such as gcc, make, qemu for virtualization, and grub for bootloader):
   ```bash
   make

3. Boot the operating system using an emulator like QEMU:
```bash
make run
```
This will start Unics in a virtual machine environment, allowing you to interact with the boot> prompt and explore the system.

## Development

4. Fork the repository on GitHub.
5. Clone your fork locally:
```bash
git clone https://github.com/your-username/Unics.git
cd Unics
```
6. Create a new branch for your changes:
```bash
git checkout -b feature-name
```
7. Make your changes, then commit and push them:
```bash
git add .
git commit -m "Description of changes"
git push origin feature-name
```
8. Create a pull request from your fork to the main repository.

## Thank you for exploring Unics! We hope it helps you learn and experiment with operating system concepts.
