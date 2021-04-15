export PATH := $(shell toolchain/path.sh)

ARCH = x86_64

CC = $(ARCH)-ackos-gcc
CXX = $(ARCH)-ackos-g++
AS = nasm
LD = $(ARCH)-ackos-ld

BIN_FOLDER = bin
SYSROOT_FOLDER = sysroot
LIBS_FOLDER = sysroot/usr/lib/static

ISONAME = ackOS
BOOTLOADER = iso

SOURCES += \
		$(wildcard kernel/*.cpp) \
		$(wildcard kernel/*/*.cpp) \
		$(wildcard arch/$(ARCH)/*.cpp) \
		$(wildcard arch/$(ARCH)/features/*.cpp)

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
		-I arch \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-fno-rtti \
		-mno-red-zone \
		-DBUILD_ARCH_$(ARCH) \
		-DBUILD_HOST_ARCH=\"$(shell uname --machine)\" \
		-DBUILD_TIME='"$(shell date)"' \
		-DBUILD_HOST_OS=\"$(shell uname --operating-system)\" \
		-std=c++20

LFLAGS += \
		-L $(LIBS_FOLDER) \
		-lc \
		-lack \
		-lutils \
		-lstdc++

QEMU_FLAGS += \
			-serial stdio

.PHONY: all qemu qemu-debug bochs check-multiboot2 clean

all: $(BIN_FOLDER)/$(ISONAME).iso

qemu: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -enable-kvm -cdrom $(BIN_FOLDER)/$(ISONAME).iso

qemu-debug: all
	@objcopy --only-keep-debug iso/$(ISONAME).bin $(BIN_FOLDER)/ackOS.sym
	@sudo qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN_FOLDER)/$(ISONAME).iso -s -S -d int -no-reboot -monitor telnet:127.0.0.1:55555,server,nowait;

bochs: all
	@bochs -q -f assets/bochsrc.bxrc

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BOOTLOADER)/$(ISONAME).bin; then \
		echo multiboot confirmed; \
	else \
		echo not multiboot2 compliant :\(; \
	fi

clean:
	@rm -rf $(BIN_FOLDER)
	@rm -f *.iso

clean-sysroot:
	@rm -rf $(SYSROOT_FOLDER)

# includes
include lib/lib.mk

$(BIN_FOLDER)/$(ISONAME).iso: $(OBJECTS) $(TARGETS)
	@echo [ linking everything ] $(LD)
	@$(LD) -n -o $(BOOTLOADER)/$(ISONAME).bin -T arch/$(ARCH)/link.ld $(OBJECTS) $(LFLAGS)
	@sudo grub-mkrescue -o $(BIN_FOLDER)/$(ISONAME).iso $(BOOTLOADER)

$(BIN_FOLDER)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN_FOLDER)/arch/$(ARCH)/%.asm.o: arch/$(ARCH)/%.asm
	@mkdir -p $(@D)
	@echo [ assemling target $@ ] Assembly
	@$(AS) -f elf64 $< -o $@

$(BIN_FOLDER)/fonts/%.o: fonts/%.psf
	@mkdir -p $(BIN_FOLDER)/fonts
	@echo [ creating font object $@ ] PC Screen Font
	@objcopy -O elf64-x86-64 -I binary $< $@
