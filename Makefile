# Toolchain
CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy

# Flags
CFLAGS = -m32 -std=gnu99 -ffreestanding -Wall -Wextra \
         -Iusr/include -I. -nostdlib -Iinclude -fno-stack-protector
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Outputs
KERNEL = unics.bin
ISO = unics.iso

# Sources
BOOT_SRC = boot/boot.s
KERNEL_SRCS = \
    sbin/init/init.c \
    usr/drivers/vga.c \
    lib/libc/string.c \
    dev/shell/shell.c \
    dev/keyboard/keyboard.c \
    usr/sbin/login/login.c \
    lib/libc/unistd.c \
    lib/libc/stdio.c \
    lib/libc/ctype.c \
    lib/libc/cdefs.c \
    lib/libc/errno.c \
    lib/libc/time.c \
    lib/libc/stdlib.c \
    bin/clear.c \
    bin/yes.c \
    bin/echo.c \
    bin/uname.c \
    bin/shutdown.c \
    bin/reboot.c \
    bin/cowsay.c \
    bin/whoami.c \
    bin/rand.c \
    bin/factor.c \
    bin/tty.c \
    bin/cpuinfo.c \
    bin/expr.c

ASM_SRCS = $(BOOT_SRC) arch/i386/cpu.s  # Added cpu.s here

# Headers
HEADERS = \
    usr/include/io.h \
    usr/include/sys/multiboot.h \
    usr/include/vga.h \
    usr/include/string.h \
    usr/include/shell.h \
    usr/include/login.h \
    usr/include/keyboard.h \
    usr/include/sys/syscall.h \
    usr/include/sys/unistd.h \
    usr/include/stddef.h \
    usr/include/stdint.h \
    usr/include/stdarg.h \
    usr/include/stdio.h \
    usr/include/stdlib.h \
    usr/include/aio.h \
    usr/include/sys/cdefs.h \
    usr/include/sys/ctype.h \
    usr/include/errno.h \
    usr/include/time.h \
    arch/i386/cpu.h \
    usr/include/sys/bootscreen.h \
    usr/include/sys/types.h

# Objects
BOOT_OBJ = $(BOOT_SRC:.s=.o)
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o) $(BOOT_OBJ) arch/i386/cpu.o  # Added cpu.o here

# Targets
.PHONY: all clean iso run

all: $(KERNEL)

iso: $(ISO)

run: $(ISO)
	qemu-system-i386 -enable-kvm -m 1024 -cdrom $(ISO)

$(KERNEL): $(KERNEL_OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@  # This rule will apply to cpu.s as well

$(ISO): $(KERNEL)
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/
	echo 'menuentry "Unics" {' > isodir/boot/grub/grub.cfg
	echo '  multiboot /boot/$(KERNEL)' >> isodir/boot/grub/grub.cfg
	echo '  boot' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir
	rm -rf isodir

clean:
	rm -f $(KERNEL_OBJS) $(KERNEL) $(ISO)
