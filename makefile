export PATH := $(shell toolchain/path.sh)

ARCH = x86_64
BOOTLOADER = limine
DIST = ackos-$(ARCH)-$(BOOTLOADER)
ISONAME = $(DIST).iso

CC := $(ARCH)-ackos-gcc
CXX := $(ARCH)-ackos-g++
AS := $(ARCH)-ackos-as
NASM := nasm
OBJCOPY := $(ARCH)-ackos-objcopy
LD := $(ARCH)-ackos-ld
AR := $(ARCH)-ackos-ar

ROOT_DIRECTORY := $(shell pwd)

BIN_FOLDER = bin
SYSROOT_FOLDER = sysroot
LIBS_FOLDER = $(BIN_FOLDER)/lib

DIST_FOLDER = config/dist/$(DIST)

SOURCES += \
		$(wildcard kernel/*.cpp) \
		$(wildcard kernel/*/*.cpp) \
		$(wildcard arch/*.cpp) \
		$(wildcard arch/$(ARCH)/*.cpp) \
		$(wildcard arch/$(ARCH)/*/*.cpp)

HEADERS += \
		$(wildcard kernel/*.h) \
		$(wildcard arch/$(ARCH)/*.h) \
		$(wildcard arch/$(ARCH)/*/*.h) \
		$(wildcard lib/*.h) \
		$(wildcard lib/*/*.h)

ASSEMBLY += \
		$(wildcard kernel/*.asm) \
		$(wildcard kernel/*/*.asm) \
		$(wildcard arch/$(ARCH)/*.asm) \
		$(wildcard arch/$(ARCH)/*/*.asm)

FONTS += \
		$(wildcard fonts/*.psf)

OBJECTS := $(ASSEMBLY:%.asm=$(BIN_FOLDER)/%.asm.o) $(SOURCES:%.cpp=$(BIN_FOLDER)/%.o) $(FONTS:%.psf=$(BIN_FOLDER)/%.o)
TARGETS :=

CFLAGS += \
		-g \
		-I lib/libc \
		-I lib/libstdc++ \
		-I lib \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-fno-rtti \
		-mno-red-zone \
		-DBUILD_ARCH_$(ARCH) \
		-DBUILD_HOST_ARCH=\"$(shell uname --machine)\" \
		-DBUILD_TIME='"$(shell date)"' \
		-DBUILD_HOST_OS=\"$(shell uname --operating-system)\" \
		-DBUILD_DISABLE_FPA \
		-std=c++20

LFLAGS += \
		-L $(LIBS_FOLDER) \
		-lc \
		-lack \
		-lstdc++

QEMU_FLAGS += \
			-serial stdio

.PHONY: all qemu qemu-debug bochs check-multiboot2 strip-symbols clean

all: $(BIN_FOLDER)/$(ISONAME)

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BIN_FOLDER)/kernel.elf; then \
		echo multiboot confirmed; \
	else \
		echo not multiboot2 compliant :\(; \
	fi

strip-symbols:
	@objcopy --only-keep-debug $(BIN_FOLDER)/kernel.elf $(BIN_FOLDER)/ackos.sym

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

# includes
include lib/lib.mk
include config/emulators/qemu.mk
include config/emulators/bochs.mk
include $(DIST_FOLDER)/build.mk

$(BIN_FOLDER)/kernel.elf: $(OBJECTS) $(TARGETS)
	@mkdir -p $(@D)
	@echo [ linking kernel ] $(LD)
	@$(LD) -n -o $@ -T arch/$(ARCH)/link.ld $(OBJECTS) $(LFLAGS)

$(BIN_FOLDER)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN_FOLDER)/arch/$(ARCH)/%.asm.o: arch/$(ARCH)/%.asm
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
