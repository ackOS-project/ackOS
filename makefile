export PATH := $(shell toolchain/path.sh)

ARCH = x86_64

CC = $(ARCH)-elf-gcc
CXX = $(ARCH)-elf-g++
AS = nasm
LD = $(ARCH)-elf-ld
BIN = bin

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

OBJECTS = $(addprefix $(BIN)/, $(ASSEMBLY:.asm=.o)) $(addprefix $(BIN)/, $(SOURCES:.cpp=.o)) $(addprefix $(BIN)/, $(FONTS:.psf=.o))

CFLAGS += \
		-g \
		-I lib/libc \
		-I lib/libstdc++ \
		-I lib \
		-I arch \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-mno-red-zone \
		-DackOS_BUILD_ARCH_$(ARCH) \
		-DackOS_BUILD_HOST_ARCH=\"$(shell uname --machine)\" \
		-DackOS_BUILD_TIME='"$(shell date)"' \
		-DackOS_BUILD_HOST_OS=\"$(shell uname --operating-system)\" \
		-std=c++2a

LFLAGS +=

QEMU_FLAGS += \
			-serial stdio

# includes
include lib/lib.mk

$(BIN)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN)/arch/$(ARCH)/%.o: arch/$(ARCH)/%.asm
	@mkdir -p $(@D)
	@echo [ assemling target $@ ] Assembly
	@$(AS) -f elf64 $< -o $@

$(BIN)/fonts/%.o: fonts/%.psf
	@mkdir -p $(BIN)/fonts
	@echo [ creating font object $@ ] PC Screen Font
	@objcopy -O elf64-x86-64 -I binary $< $@

all: $(OBJECTS)
	@echo [ linking everything ] $(LD)
	@$(LD) -n -o $(BOOTLOADER)/$(ISONAME).bin -T arch/$(ARCH)/link.ld $(OBJECTS) $(LFLAGS)
	@sudo grub-mkrescue -o $(BIN)/$(ISONAME).iso $(BOOTLOADER)

qemu: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -enable-kvm -cdrom $(BIN)/$(ISONAME).iso

qemu-debug: all
	@objcopy --only-keep-debug iso/$(ISONAME).bin $(BIN)/ackOS.sym
	@sudo qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN)/$(ISONAME).iso -d int -no-reboot -monitor telnet:127.0.0.1:55555,server,nowait;

bochs: all
	@bochs -q -f assets/bochsrc.bxrc

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BOOTLOADER)/$(ISONAME).bin; then \
		echo multiboot confirmed; \
	else \
		echo the kernel is not multiboot2 compliant :\(; \
	fi

clean:
	@rm -rf $(BIN)
	@rm -f *.iso