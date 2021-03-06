export PATH := $(shell toolchain/path.sh)

ARCH = x86_64
BOOTLOADER = limine
DIST = ackos-$(ARCH)-$(BOOTLOADER)
ISONAME = $(DIST).iso

CC = $(ARCH)-ackos-gcc
CXX = $(ARCH)-ackos-g++
AS = $(ARCH)-ackos-as
NASM = nasm
LD = $(ARCH)-ackos-ld

BIN_FOLDER = bin
SYSROOT_FOLDER = sysroot
LIBS_FOLDER = sysroot/usr/lib/static

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
		-mgeneral-regs-only \
		-std=c++20

LFLAGS += \
		-L $(LIBS_FOLDER) \
		-lc \
		-lack \
		-lutils \
		-lstdc++

QEMU_FLAGS += \
			-serial stdio

.PHONY: all qemu qemu-debug bochs check-multiboot2 strip-symbols clean

all: $(BIN_FOLDER)/$(ISONAME)

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BIN_FOLDER)/ackos.elf; then \
		echo multiboot confirmed; \
	else \
		echo not multiboot2 compliant :\(; \
	fi

strip-symbols:
	@objcopy --only-keep-debug $(BIN_FOLDER)/ackos.elf $(BIN_FOLDER)/ackos.sym

clean:
	@rm -rf $(BIN_FOLDER)
	@rm -f *.iso

clean-sysroot:
	@rm -rf $(SYSROOT_FOLDER)

# includes
include lib/lib.mk
include config/emulators/qemu.mk
include config/emulators/bochs.mk
include $(DIST_FOLDER)/build.mk

$(BIN_FOLDER)/ackos.elf: $(OBJECTS) $(TARGETS)
	@mkdir -p $(@D)
	@echo [ linking everything ] $(LD)
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
