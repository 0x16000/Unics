# Makefile for Unics OS Project

# --- Configuration ---
KERNEL_NAME    := unics
ARCH           := i386
ARCHDIR        := arch/$(ARCH)

# --- Toolchain Configuration ---
CROSS_COMPILE ?= i686-linux-gnu-
CC             := $(CROSS_COMPILE)gcc
AS             := nasm
LD             := $(CROSS_COMPILE)ld
OBJCOPY        := $(CROSS_COMPILE)objcopy
GRUB_MKRESCUE  := grub-mkrescue
QEMU           := qemu-system-i386

# Check for cross-compiler availability
ifeq ($(shell command -v $(CC) 2>/dev/null),)
$(error "Cross-compiler $(CC) not found. Please run: sudo apt install gcc-i686-linux-gnu g++-i686-linux-gnu binutils-i686-linux-gnu")
endif

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

BASE_CFLAGS    := -std=gnu99 -ffreestanding $(OPTIMIZATION) $(WARNINGS) $(SECURITY) \
                  -I$(INCDIR) -I. -nostdlib -fno-common -fno-builtin \
                  -fno-omit-frame-pointer -ggdb3

CFLAGS_KERNEL  := $(BASE_CFLAGS) -D_KERNEL
CFLAGS_USERLAND := $(BASE_CFLAGS)

ASFLAGS        := -f elf32 -F dwarf -g

LDFLAGS := -nostdlib -nostartfiles -T linker.ld -Wl,--gc-sections -Wl,-Map=$(BUILDDIR)/$(KERNEL_NAME).map

QEMU_OPTS      ?= -enable-kvm -m 1024 -serial stdio -vga std

MKDIR_P        := mkdir -p

# --- Source Discovery ---
# Scan all C sources once, then split by directory prefix
ALL_C_SRCS     := $(shell find kern arch/$(ARCH) sbin usr lib dev bin -name '*.c' 2>/dev/null)

KERNEL_SRCS    := $(filter kern/% arch/$(ARCH)/%, $(ALL_C_SRCS))
# Exclude sbin/init/init.c from userland for special build
USERLAND_SRCS  := $(filter-out $(KERNEL_SRCS) sbin/init/init.c, $(ALL_C_SRCS))

BOOT_SRC       := $(BOOTDIR)/boot.s
ASM_SRCS       := $(BOOT_SRC) $(wildcard $(ARCHDIR)/*.s)

HEADERS        := $(shell find $(INCDIR) -name '*.h' 2>/dev/null) \
                  $(wildcard $(ARCHDIR)/*.h)

# --- Object Files ---
ASM_OBJS       := $(patsubst %.s,$(BUILDDIR)/%.o,$(ASM_SRCS))
KERNEL_OBJS    := $(patsubst %.c,$(BUILDDIR)/%.o,$(KERNEL_SRCS))
USERLAND_OBJS  := $(patsubst %.c,$(BUILDDIR)/%.o,$(USERLAND_SRCS))
INIT_OBJ       := $(BUILDDIR)/sbin/init/init.o

OBJS           := $(ASM_OBJS) $(KERNEL_OBJS) $(USERLAND_OBJS) $(INIT_OBJ)
DEPS           := $(OBJS:.o=.d)

# --- Phony Targets ---
.PHONY: all clean distclean iso run run-nokvm debug prepare help

# --- Dependency Inclusion (early) ---
-include $(DEPS)

# --- Default Target ---
all: $(BUILDDIR)/$(KERNEL_BIN)

# --- Link Kernel ---
$(BUILDDIR)/$(KERNEL_BIN): $(OBJS) linker.ld | $(BUILDDIR)
	@echo "[LD] Linking $@"
	$(CC) -nostdlib -nostartfiles -T linker.ld -Wl,--gc-sections -Wl,-Map=$(BUILDDIR)/$(KERNEL_NAME).map -o $@ $(OBJS) -lgcc
	@echo "[INFO] Kernel size: $$(stat -c%s $@) bytes"

# --- Compile kernel C sources ---
$(BUILDDIR)/kern/%.o: kern/%.c $(HEADERS) | $(BUILDDIR)/kern
	@$(MKDIR_P) $(@D)
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

$(BUILDDIR)/arch/$(ARCH)/%.o: arch/$(ARCH)/%.c $(HEADERS) | $(BUILDDIR)/arch/$(ARCH)
	@$(MKDIR_P) $(@D)
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

# --- Special case: Compile sbin/init/init.c as kernel ---
$(BUILDDIR)/sbin/init/init.o: sbin/init/init.c $(HEADERS) | $(BUILDDIR)/sbin/init
	@$(MKDIR_P) $(@D)
	@echo "[CC] (kernel) Compiling $<"
	$(CC) $(CFLAGS_KERNEL) -MMD -MP -c $< -o $@

# --- Compile userland C sources ---
$(BUILDDIR)/sbin/%.o: sbin/%.c $(HEADERS) | $(BUILDDIR)/sbin
	@$(MKDIR_P) $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/usr/%.o: usr/%.c $(HEADERS) | $(BUILDDIR)/usr
	@$(MKDIR_P) $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/lib/%.o: lib/%.c $(HEADERS) | $(BUILDDIR)/lib
	@$(MKDIR_P) $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/dev/%.o: dev/%.c $(HEADERS) | $(BUILDDIR)/dev
	@$(MKDIR_P) $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

$(BUILDDIR)/bin/%.o: bin/%.c $(HEADERS) | $(BUILDDIR)/bin
	@$(MKDIR_P) $(@D)
	@echo "[CC] (userland) Compiling $<"
	$(CC) $(CFLAGS_USERLAND) -MMD -MP -c $< -o $@

# --- Assemble assembly files ---
$(BUILDDIR)/%.o: %.s | $(BUILDDIR)
	@$(MKDIR_P) $(@D)
	@echo "[AS] Assembling $<"
	$(AS) $(ASFLAGS) $< -o $@

# --- Directory Creation ---
$(BUILDDIR):
	@$(MKDIR_P) $@

$(BUILDDIR)/sbin:
	@$(MKDIR_P) $@

$(BUILDDIR)/usr:
	@$(MKDIR_P) $@

$(BUILDDIR)/lib:
	@$(MKDIR_P) $@

$(BUILDDIR)/dev:
	@$(MKDIR_P) $@

$(BUILDDIR)/bin:
	@$(MKDIR_P) $@

$(BUILDDIR)/arch:
	@$(MKDIR_P) $@

$(BUILDDIR)/arch/$(ARCH):
	@$(MKDIR_P) $@

$(BUILDDIR)/kern:
	@$(MKDIR_P) $@

$(BUILDDIR)/sbin/init:
	@$(MKDIR_P) $@

# --- ISO Preparation ---
prepare: $(BUILDDIR)/$(KERNEL_BIN)
	@echo "[PREP] Preparing ISO directory structure"
	@rm -rf $(ISODIR)
	@$(MKDIR_P) $(ISODIR)/boot/grub
	@cp $(BUILDDIR)/$(KERNEL_BIN) $(ISODIR)/boot/
	@printf 'menuentry "%s" {\n  multiboot /boot/%s\n  boot\n}\n' \
		"$(KERNEL_NAME)" "$(KERNEL_BIN)" > $(ISODIR)/boot/grub/grub.cfg

# --- ISO Creation ---
iso: prepare
	@echo "[ISO] Creating ISO image: $(ISO_IMAGE)"
	@$(GRUB_MKRESCUE) -o $(ISO_IMAGE) $(ISODIR) > /dev/null
	@rm -rf $(ISODIR)
	@echo "[INFO] ISO created: $(ISO_IMAGE)"

# --- QEMU Targets ---
QEMU_BASE := $(QEMU) -m 1024 -cdrom $(ISO_IMAGE) -serial stdio

run: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching with KVM acceleration"
	@$(QEMU_BASE) -enable-kvm -vga std

debug: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching with GDB server (port 1234)"
	@$(QEMU_BASE) -s -S

run-nokvm: iso
	@command -v $(QEMU) >/dev/null 2>&1 || { \
		echo >&2 "QEMU not found. Please install QEMU."; exit 1; }
	@echo "[QEMU] Launching without KVM acceleration"
	@$(QEMU_BASE)

# --- Clean Targets ---
clean:
	@echo "[CLEAN] Removing build artifacts"
	@rm -rf $(BUILDDIR)

distclean: clean
	@echo "[CLEAN] Removing all generated files"
	@rm -f $(ISO_IMAGE)
	@rm -rf $(ISODIR)

# --- Help Target ---
help:
	@echo "Available targets:"
	@echo "  all       - Build the kernel (default target)"
	@echo "  iso       - Create bootable ISO image"
	@echo "  run       - Run in QEMU with KVM acceleration"
	@echo "  run-nokvm - Run in QEMU without KVM acceleration"
	@echo "  debug     - Run in QEMU with GDB server (port 1234)"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all generated files"


