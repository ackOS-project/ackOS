export PATH := $(shell toolchain/path.sh)

.DEFAULT_GOAL := all

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
CREATE_DIRS = @mkdir -p $(@D)

ifeq ($(V),true)
	override V :=
else
	V := @
endif

SYSTEM := ackos
ARCH := x86_64
BOOTLOADER := limine
DIST := $(SYSTEM)-$(ARCH)-$(BOOTLOADER)

CC := $(ARCH)-$(SYSTEM)-gcc
CXX := $(ARCH)-$(SYSTEM)-g++
AS := $(ARCH)-$(SYSTEM)-as
OBJCOPY := $(ARCH)-$(SYSTEM)-objcopy
OBJDUMP := $(ARCH)-$(SYSTEM)-objdump
LD := $(ARCH)-$(SYSTEM)-ld
AR := $(ARCH)-$(SYSTEM)-ar

VM_MEMORY := 128
VM_LOGFILE := ackos.log

BIN_DIR := bin
OBJ_DIR := obj

CFLAGS := -g \
		  -O2 \
		  -ffreestanding \
		  -std=c11 \
		  -Wall \
		  -Werror \
		  -Wstrict-prototypes
ASFLAGS :=
LDFLAGS := -nostdlib -static

KERNEL_BIN =
OS_IMAGE =

SOURCES =

include kernel/build.mk
include config/emulators/qemu.mk
include config/emulators/bochs.mk
include config/dist/$(DIST)/build.mk

all: $(OS_IMAGE)

clean:
	@rm -rf $(BIN_DIR)
	@rm -rf $(OBJ_DIR)

$(OBJ_DIR)/%.c.o: %.c
	$(CREATE_DIRS)
	@echo "Building $<"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.s.o: %.s
	$(CREATE_DIRS)
	@echo "Building $<"
	$(V)$(AS) $(ASFLAGS) -c $< -o $@

