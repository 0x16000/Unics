# Makefile for Unics OS Project - Optimized and Enhanced

# --- Configuration ---
KERNEL_NAME    := unics
ARCH           := i386
ARCHDIR        := arch/$(ARCH)

# --- Toolchain Configuration ---
CC             := gcc
AS             := nasm
LD             := ld
OBJCOPY        := objcopy
GRUB_MKRESCUE  := grub-mkrescue
QEMU           := qemu-system-i386  # Works fine for i686 code

# --- Directory Structure ---
SRCDIR         := .
BOOTDIR        := boot
INCDIR         := usr/include
ISODIR         := isodir
BUILDDIR       := build

# --- Output Files ---
KERNEL_BIN     := $(KERNEL_NAME).bin
ISO_IMAGE      := $(KERNEL_NAME).iso

# --- Compiler/Linker Flags ---
OPTIMIZATION   := -O2
WARNINGS       := -Wall -Wextra -Werror=implicit-function-declaration \
                  -Werror=incompatible-pointer-types -Werror=int-conversion
SECURITY       := -fno-stack-protector -fno-PIE -fno-PIC

BASE_CFLAGS    := -m32 -march=i686 -std=gnu99 -ffreestanding $(OPTIMIZATION) $(WARNINGS) $(SECURITY) \
                  -I$(INCDIR) -I. -nostdlib -fno-common -fno-builtin \
                  -fno-omit-frame-pointer -ggdb3

CFLAGS_KERNEL  := $(BASE_CFLAGS) -D_KERNEL
CFLAGS_USERLAND := $(BASE_CFLAGS)

ASFLAGS        := -f elf32 -F dwarf -g
LDFLAGS        := -m32 -nostdlib -nostartfiles -T linker.ld -Wl,--gc-sections \
                  -Wl,-Map=$(BUILDDIR)/$(KERNEL_NAME).map

QEMU_OPTS      ?= -enable-kvm -m 1024 -serial stdio -vga std

# --- Source Discovery ---
BOOT_SRC       := $(BOOTDIR)/boot.s
ASM_SRCS       := $(BOOT_SRC) $(wildcard $(ARCHDIR)/*.s)

# Kernel C sources: inside kern/ and arch/$(ARCH)/
KERNEL_SRCS    := $(shell find kern arch/$(ARCH) -name '*.c' 2>/dev/null)

# Userland C sources: everything else under sbin usr lib dev bin excluding kernel src
USERLAND_SRCS  := $(filter-out $(KERNEL_SRCS), $(shell find sbin usr lib dev bin -name '*.c' 2>/dev/null))

HEADERS        := $(shell find $(INCDIR) -name '*.h' 2>/dev/null) \
                  $(wildcard $(ARCHDIR)/*.h)

# --- Object Files ---
ASM_OBJS       := $(patsubst %.s,$(BUILDDIR)/%.o,$(ASM_SRCS))
KERNEL_OBJS    := $(patsubst %.c,$(BUILDDIR)/%.o,$(KERNEL_SRCS))
USERLAND_OBJS  := $(patsubst %.c,$(BUILDDIR)/%.o,$(USERLAND_SRCS))

OBJS           := $(ASM_OBJS) $(KERNEL_OBJS) $(USERLAND_OBJS)
DEPS           := $(OBJS:.o=.d)

# --- Phony Targets ---
.PHONY: all clean iso run debug run-nokvm prepare distclean help

# --- Default Target ---
all: $(BUILDDIR)/$(KERNEL_BIN)

# --- Link Kernel ---
$(BUILDDIR)/$(KERNEL_BIN): $(OBJS) linker.ld | $(BUILDDIR)
	@echo "[LD] Linking $@"
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lgcc
	@echo "[INFO] Kernel size: $$(stat -c%s $@) bytes"

# --- Compile kernel C sources ---
$(BUILDDIR)/kern/%.o: kern/%.c $(HEADERS) | $(BUILDDIR)
	@mkdir -p $(@D)
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

$(BUILDDIR)/arch/$(ARCH)/%.o: arch/$(ARCH)/%.c $(HEADERS) | $(BUILDDIR)
	@mkdir -p $(@D)
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

# --- Compile sbin/init/init.c as kernel (special case) ---
$(BUILDDIR)/sbin/init:
	@mkdir -p $@

$(BUILDDIR)/sbin/init/init.o: sbin/init/init.c $(HEADERS) | $(BUILDDIR)/sbin/init
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

# --- Compile other sbin userland C sources (excluding sbin/init/init.c) ---
$(BUILDDIR)/sbin/%.o: sbin/%.c $(HEADERS) | $(BUILDDIR)/sbin
	if [ "$<" != "sbin/init/init.c" ]; then \
		echo "[CC] (userland) Compiling $<"; \
		$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@; \
	fi

# --- Compile userland C sources (usr, lib, dev, bin) ---
$(BUILDDIR)/usr/%.o: usr/%.c $(HEADERS) | $(BUILDDIR)/usr
	@mkdir -p $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/lib/%.o: lib/%.c $(HEADERS) | $(BUILDDIR)/lib
	@mkdir -p $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/dev/%.o: dev/%.c $(HEADERS) | $(BUILDDIR)/dev
	@mkdir -p $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/bin/%.o: bin/%.c $(HEADERS) | $(BUILDDIR)/bin
	@mkdir -p $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

# --- Assemble assembly files ---
$(BUILDDIR)/%.o: %.s | $(BUILDDIR)
	@mkdir -p $(@D)
	@echo "[AS] Assembling $<"
	$(AS) $(ASFLAGS) $< -o $@

# --- Directory Creation ---
$(BUILDDIR):
	@mkdir -p $@

$(BUILDDIR)/sbin:
	@mkdir -p $@

$(BUILDDIR)/usr:
	@mkdir -p $@

$(BUILDDIR)/lib:
	@mkdir -p $@

$(BUILDDIR)/dev:
	@mkdir -p $@

$(BUILDDIR)/bin:
	@mkdir -p $@

$(BUILDDIR)/arch:
	@mkdir -p $@

$(BUILDDIR)/arch/$(ARCH):
	@mkdir -p $@

$(BUILDDIR)/kern:
	@mkdir -p $@

# --- ISO Preparation ---
prepare: $(BUILDDIR)/$(KERNEL_BIN)
	@echo "[PREP] Preparing ISO directory structure"
	@rm -rf $(ISODIR)
	@mkdir -p $(ISODIR)/boot/grub
	@cp $(BUILDDIR)/$(KERNEL_BIN) $(ISODIR)/boot/
	@printf 'menuentry "%s" {\n  multiboot /boot/%s\n  boot\n}\n' \
		"$(KERNEL_NAME)" "$(KERNEL_BIN)" > $(ISODIR)/boot/grub/grub.cfg

# --- ISO Creation ---
iso: prepare
	@echo "[ISO] Creating ISO image: $(ISO_IMAGE)"
	@$(GRUB_MKRESCUE) -o $(ISO_IMAGE) $(ISODIR) >/dev/null 2>&1
	@rm -rf $(ISODIR)
	@echo "[INFO] ISO created: $(ISO_IMAGE)"

# --- QEMU Targets ---
run: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching with KVM acceleration"
	@$(QEMU) $(QEMU_OPTS) -cdrom $(ISO_IMAGE)

debug: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching with GDB server (port 1234)"
	@$(QEMU) -m 1024 -cdrom $(ISO_IMAGE) -s -S -serial stdio

run-nokvm: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching without KVM acceleration"
	@$(QEMU) -m 1024 -cdrom $(ISO_IMAGE) -serial stdio

# --- Clean Targets ---
clean:
	@echo "[CLEAN] Removing build artifacts"
	@rm -rf $(BUILDDIR)

distclean: clean
	@echo "[CLEAN] Removing all generated files"
	@rm -f $(ISO_IMAGE)
	@rm -rf $(ISODIR)

# --- Dependency Inclusion ---
-include $(DEPS)
