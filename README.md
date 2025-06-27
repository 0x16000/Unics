![Image](https://github.com/user-attachments/assets/528b386e-fab2-4be7-97d9-197d865b2fb4)

## Unics Source:
This is the top level of the Unics source directory.

Unics is a Unix-Like Operating system, Unics tends to focus on usability and security.
A new kernel developed entirely from scratch.

For copyright information, please see [the LICENSE file](LICENSE) in this directory.

The Makefile in this directory builds the whole system from the source tree.

For information on the CPU architecture supported by Unics: i686.

For official Unics bootable images, [get to RELEASE tab](https://github.com/0x16000/Unics/releases/tag/Open).

## Source Roadmap

| Directory | Description |
|----------|-------------|
| `bin`    | System/user commands. |
| `arch`   | Processor-dependent software. (GCC Machine Description) |
| `boot`   | Boot-specific code. |
| `build`  | Build process artifacts (e.g., `.d` files). |
| `dev`    | Device driver files (e.g., shell, keyboard, etc.). |
| `lib`    | Libraries (e.g., C standard library). |
| `sbin`   | System-dependent software. |
| `usr`    | User space files (e.g., `include/`, drivers, `sbin/`). |
| `doc`    | Visible in folders / Documentation (eg., `usr/drivers`) |

## Install necessary dependencies for building the kernel
Just run the file:
```bash
chmod +x dependencies.pl
./dependencies.pl
```
Or:
```bash
perl dependencies.pl
```

## Compile kernel
Compiling is straight-forward.
-
To compile run:
```bash
make
```

To produce an ISO (bootable image)
```bash
make iso
```

Finally run Unics
```bash
make run
```

## Contribute
We gladly accept contributors to the Unics kernel and grow our community.
For now please ONLY write in C, assembly only if it is software that cannot be written in C.

Thank you for choosing Unics First Edition. Have a nice day.
