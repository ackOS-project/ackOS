export PATH := $(shell toolchain/path.sh)

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ARCH = x86_64
BOOTLOADER = limine
DIST = ackos-$(ARCH)-$(BOOTLOADER)
ISONAME = $(DIST).iso

TOOLCHAIN_LIBGCC = $(shell toolchain/path.sh --option=libgcc --target=$(ARCH)-ackos)

CC := $(ARCH)-ackos-gcc
CXX := $(ARCH)-ackos-g++
AS := $(ARCH)-ackos-as
NASM := nasm
OBJCOPY := $(ARCH)-ackos-objcopy
OBJDUMP := $(ARCH)-ackos-objdump
LD := $(ARCH)-ackos-ld
AR := $(ARCH)-ackos-ar

ROOT_DIRECTORY := $(shell pwd)

BIN_FOLDER = bin
SYSROOT_FOLDER = sysroot
LIBS_FOLDER = $(BIN_FOLDER)/lib

DIST_FOLDER = config/dist/$(DIST)

FONTS += \
		$(wildcard fonts/*.psf)

TARGETS :=

SANITISATION_ENABLED := yes
DEBUG_ENABLED := yes
OPTIMISATION_LEVEL := 3

CFLAGS += \
		-I lib/libc \
		-I lib/libstdc++ \
		-I lib \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-fno-rtti \
		-O$(OPTIMISATION_LEVEL) \
		-mno-red-zone \
		-DBUILD_ARCH_$(ARCH) \
		-DBUILD_HOST_ARCH=\"$(shell uname --machine)\" \
		-DBUILD_TIME='"$(shell date)"' \
		-DBUILD_HOST_OS=\"$(shell uname --operating-system)\" \
		-std=c++20

ifeq ($(SANITISATION_ENABLED), yes)
	CFLAGS += -fsanitize=undefined
endif

ifeq ($(DEBUG_ENABLED), yes)
	CFLAGS += -g
	TARGETS += strip-symbols
endif

LFLAGS += \
		-L $(LIBS_FOLDER) \
		-lc \
		-lack \
		-lstdc++ \
		-L $(TOOLCHAIN_LIBGCC) \
		-lgcc

VM_LOGFILE := ackos.log
VM_MEMORY := 64

DEBUG_SYMBOL_FILE := $(BIN_FOLDER)/ackos.sym
DEBUG_BREAKPOINT := kmain

SYMLIST_FILE := $(BIN_FOLDER)/symlist.cpp

.PHONY: all qemu qemu-debug bochs build-sysroot check-multiboot2 strip-symbols clean clean-sysroot print-log

all: $(BIN_FOLDER)/$(ISONAME)

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BIN_FOLDER)/kernel.elf; then \
		echo multiboot confirmed; \
	else \
		echo not multiboot2 compliant :\(; \
	fi

strip-symbols:
	@mkdir -p $(BIN_FOLDER)
	@make --no-print-directory $(BIN_FOLDER)/kernel.elf DEBUG_ENABLED=no >/dev/null
	@echo "[ stripping symbols ] objcopy"
	@$(OBJCOPY) --only-keep-debug $(BIN_FOLDER)/kernel.elf $(DEBUG_SYMBOL_FILE)
	@rm -rf $(BIN_FOLDER)/symlist.cpp; \
	 export SYMFILE="$(shell mktemp)"; \
	 export ADDRFILE="$(shell mktemp)"; \
	 $(OBJDUMP) -t bin/kernel.elf | sed '/\bd\b/d' | sort | grep "\.text" | awk 'NF{ print $$NF }' > $$SYMFILE; \
	 $(OBJDUMP) -t bin/kernel.elf | sed '/\bd\b/d' | sort | grep "\.text" | cut -d' ' -f1 > $$ADDRFILE; \
	 printf "#include <cstdint>\n\nstruct symtab_entry_t\n{\n    char* name;\n    uintptr_t addr;\n};\n\nsymtab_entry_t _kernel_symbol_table[] =\n{\n" >> $(SYMLIST_FILE); \
	 paste -d'$$' "$$SYMFILE" "$$ADDRFILE" | sed 's/^/    { "/g' | sed 's/\$$/", 0x/g' | sed 's/$$/ },/g' >> $(SYMLIST_FILE); \
	 printf "    { \"\", 0xffffffffffffffff }\n};\n" >> $(SYMLIST_FILE); \
	 rm $$SYMFILE; \
	 rm $$ADDRFILE

build-sysroot:
	@mkdir -p sysroot
	@mkdir -p sysroot/usr/lib/static sysroot/usr/lib/headers

	@cd lib; cp --parents $(shell cd lib; find -name \*.h*) $(ROOT_DIRECTORY)/sysroot/usr/lib/headers
	@cp $(LIBS_FOLDER)/* sysroot/usr/lib/static 2>/dev/null || :

clean:
	@rm -rf $(BIN_FOLDER)
	@rm -rf sysroot
	@rm -f *.iso

clean-sysroot:
	@rm -rf $(SYSROOT_FOLDER)

print-log:
	@cat ackos.log 2>/dev/null

# includes
include kernel/kernel.mk
include lib/lib.mk
include config/emulators/qemu.mk
include config/emulators/bochs.mk
include config/debug/gdb.mk
include $(DIST_FOLDER)/build.mk

$(BIN_FOLDER)/kernel.elf: $(KERNEL_OBJECTS) $(TARGETS)
	@mkdir -p $(@D)
	@echo [ linking kernel ] $(LD)
	@if [ -f "$(SYMLIST_FILE)" ]; \
	 then \
	 	$(CXX) -c "$(SYMLIST_FILE)" -o $(BIN_FOLDER)/symlist.o -Wno-write-strings $(CFLAGS); \
	 else \
	 	export TMP=$(shell mktemp /tmp/tmp.XXXXXXXX.cpp); \
	 	printf "#include <cstdint>\n\nstruct symtab_entry_t\n{\n    char* name;\n    uintptr_t addr;\n};\n\nsymtab_entry_t _kernel_symbol_table[] = { { \"\", 0xffffffffffffffff } };\n" > $$TMP; \
		$(CXX) -Wno-write-strings -c $$TMP -o $(BIN_FOLDER)/symlist.o $(CFLAGS) && \
		rm $$TMP; \
	 fi
	@$(LD) -n -o $@ -T kernel/arch/$(ARCH)/link.ld $(BIN_FOLDER)/symlist.o $(KERNEL_OBJECTS) $(LFLAGS)

$(BIN_FOLDER)/%.o: %.cpp $(KERNEL_HEADERS)
	@mkdir -p $(@D)
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN_FOLDER)/%.32.o: %.32.cpp $(KERNEL_HEADERS)
	@mkdir -p $(@D)
	@echo [ compiling 32-bit target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS) -m32
	@$(OBJCOPY) -O elf64-x86-64 $@ $@

$(BIN_FOLDER)/%.asm.o: %.asm
	@mkdir -p $(@D)
	@echo [ assemling target $@ ] Assembly
	@$(NASM) -f elf64 $< -o $@

$(BIN_FOLDER)/fonts/%.o: fonts/%.psf
	@mkdir -p $(BIN_FOLDER)/fonts
	@echo [ creating font object $@ ] PC Screen Font
	@objcopy -O elf64-x86-64 -I binary $< $@

$(BIN_FOLDER)/ramdisk.tar.gz: build-sysroot
	@echo [ creating ramdisk $@ ] tar
	@cd $(SYSROOT_FOLDER); tar -czf $(ROOT_DIRECTORY)/$@ .
