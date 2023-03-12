export PATH := $(shell toolchain/path.sh)

.DEFAULT_GOAL := all

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
CREATE_DIRS = @mkdir -p $(@D)

ifeq ($(V),true)
	override V :=
else
	override V := @
endif

# release - no debugging info, maximum optimisation
# build - minimal debug info, maximum optimisation
# debug - includes full debug info including verbose logging, no optimisation
ifeq ($(filter release build debug,$(CONFIG)),)
	override CONFIG := debug
endif

SYSTEM := ackos
VERSION := 0
ARCH := x86_64
BOOTLOADER := limine
DIST := $(SYSTEM)-$(ARCH)-$(BOOTLOADER)
OS_DIST := $(SYSTEM)-v$(VERSION)-$(CONFIG)-$(ARCH)-$(BOOTLOADER)

CC := $(ARCH)-$(SYSTEM)-gcc
CXX := $(ARCH)-$(SYSTEM)-g++
AS := $(ARCH)-$(SYSTEM)-as
OBJCOPY := $(ARCH)-$(SYSTEM)-objcopy
OBJDUMP := $(ARCH)-$(SYSTEM)-objdump
LD := $(ARCH)-$(SYSTEM)-ld
AR := $(ARCH)-$(SYSTEM)-ar

VM_MEMORY := 1512
VM_LOGFILE := ackos.log

BIN_DIR := bin
OBJ_DIR := obj/$(OS_DIST)

CFLAGS := -ffreestanding \
		  -std=c11 \
		  -Wall \
		  -Werror \
		  -Wstrict-prototypes \
		  -Wno-unused-variable \
		  --warn-no-unused-function \
		  -D BUILD_CONFIG_$(shell echo $(CONFIG) | tr a-z A-Z) \
		  -D BUILD_VERSION=$(VERSION)
ASFLAGS :=
LDFLAGS := -nostdlib -static

ifneq ($(filter release build,$(CONFIG)),)
	CFLAGS += -O3
endif

ifneq ($(filter debug,$(CONFIG)),)
	CFLAGS += -g
endif

KERNEL_BIN =
KERNEL_SYMS =
OS_IMAGE =

SOURCES =

include kernel/build.mk
include config/emulators/qemu.mk
include config/emulators/bochs.mk
include config/dist/$(DIST)/build.mk

ifneq ($(filter debug,$(CONFIG)),)
	include config/debug/gdb.mk
endif

all: $(OS_IMAGE)

clean:
	@rm -rf bin
	@rm -rf obj

$(OBJ_DIR)/%.c.o: %.c
	$(CREATE_DIRS)
	@echo "Building $<"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.s.o: %.s
	$(CREATE_DIRS)
	@echo "Building $<"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

