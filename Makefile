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

CFLAGS         := -m32 -march=i686 -std=gnu99 -ffreestanding $(OPTIMIZATION) $(WARNINGS) $(SECURITY) \
                  -I$(INCDIR) -I. -nostdlib -fno-common -fno-builtin \
                  -fno-omit-frame-pointer -ggdb3

ASFLAGS        := -f elf32 -F dwarf -g
LDFLAGS        := -m32 -nostdlib -nostartfiles -T linker.ld -Wl,--gc-sections \
                  -Wl,-Map=$(BUILDDIR)/$(KERNEL_NAME).map

QEMU_OPTS      ?= -enable-kvm -m 1024 -serial stdio -vga std

# --- Automatic Source Discovery ---
BOOT_SRC       := $(BOOTDIR)/boot.s
ASM_SRCS       := $(BOOT_SRC) $(wildcard $(ARCHDIR)/*.s)
KERNEL_SRCS := $(shell find sbin usr lib dev bin kern -name '*.c' 2>/dev/null)
HEADERS        := $(shell find $(INCDIR) -name '*.h' 2>/dev/null) \
                  $(wildcard $(ARCHDIR)/*.h)

# --- Build Artifacts ---
ASM_OBJS       := $(patsubst %.s,$(BUILDDIR)/%.o,$(ASM_SRCS))
C_OBJS         := $(patsubst %.c,$(BUILDDIR)/%.o,$(KERNEL_SRCS))
OBJS           := $(ASM_OBJS) $(C_OBJS)
DEPS           := $(C_OBJS:.o=.d)

# --- Phony Targets ---
.PHONY: all clean iso run debug run-nokvm prepare distclean help

# --- Default Target ---
all: $(BUILDDIR)/$(KERNEL_BIN)

# --- Help Target ---
help:
	@echo "Available targets:"
	@echo "  all       - Build the kernel (default target)"
	@echo "  clean     - Remove most build artifacts"
	@echo "  distclean - Remove all build artifacts and ISOs"
	@echo "  iso       - Create bootable ISO image"
	@echo "  run       - Run in QEMU with KVM acceleration"
	@echo "  run-nokvm - Run in QEMU without KVM acceleration"
	@echo "  debug     - Run in QEMU with GDB server"
	@echo "  prepare   - Prepare ISO directory structure"
	@echo "  help      - Show this help message"

# --- Build Rules ---
$(BUILDDIR)/$(KERNEL_BIN): $(OBJS) linker.ld | $(BUILDDIR)
	@echo "[LD] Linking $@"
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lgcc
	@echo "[INFO] Kernel size: $$(stat -c%s $@) bytes"

# --- Pattern Rules ---
$(BUILDDIR)/%.o: %.c $(HEADERS) | $(BUILDDIR)
	@mkdir -p $(@D)
	@echo "[CC] Compiling $<"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILDDIR)/%.o: %.s | $(BUILDDIR)
	@mkdir -p $(@D)
	@echo "[AS] Assembling $<"
	$(AS) $(ASFLAGS) $< -o $@

# --- Directory Creation ---
$(BUILDDIR):
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
