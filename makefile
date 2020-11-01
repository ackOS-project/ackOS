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
		$(wildcard arch/$(ARCH)/*.cpp) \
		$(wildcard arch/$(ARCH)/features/*.cpp)

HEADERS += \
		$(wildcard kernel/*.h) \
		$(wildcard lib/*.h) \
		$(wildcard lib/*/*.h)

ASSEMBLY += \
		$(wildcard kernel/*.asm) \
		$(wildcard kernel/*/*.asm) \
		$(wildcard arch/$(ARCH)/*.asm)

FONTS += \
		$(wildcard fonts/*.psf)

OBJECTS = $(addprefix $(BIN)/, $(ASSEMBLY:.asm=.o)) $(addprefix $(BIN)/, $(SOURCES:.cpp=.o)) $(addprefix $(BIN)/, $(FONTS:.psf=.o))

CFLAGS += \
		-I lib/libc \
		-I lib/libstdc++ \
		-I lib \
		-I arch \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-DackOS_BUILD_ARCH_$(ARCH) \
		-DackOS_BUILD_HOST_ARCH=$(sh uname --machine) \
		-DackOS_BUILD_HOST_OS=$(sh uname --operating-system) \
		-std=c++2a

LFLAGS +=

QEMU_FLAGS += \
			-enable-kvm \
			-serial stdio

# includes
include lib/lib.mk

$(BIN)/kernel/%.o: kernel/%.cpp $(HEADERS)
	@mkdir -p $(BIN)/kernel
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN)/arch/$(ARCH)/%.o: arch/$(ARCH)/%.asm
	@mkdir -p $(BIN)/arch/$(ARCH)
	@echo [ assemling target $@ ] Assembly
	@$(AS) -f elf64 $< -o $@

$(BIN)/arch/$(ARCH)/%.o: arch/$(ARCH)/%.cpp
	@mkdir -p $(BIN)/arch/$(ARCH)
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN)/arch/$(ARCH)/features/%.o: arch/$(ARCH)/features/%.cpp
	@mkdir -p $(BIN)/arch/$(ARCH)/features
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN)/fonts/%.o: fonts/%.psf
	@mkdir -p $(BIN)/fonts
	@echo [ creating font object $@ ] PC Screen Font
	@objcopy -O elf64-x86-64 -I binary $< $@

all: $(OBJECTS)
	@echo [ linking everything ] $(LD)
	@$(LD) -n -o $(BOOTLOADER)/$(ISONAME).bin -T linker.ld $(OBJECTS) $(LFLAGS)
	@sudo grub-mkrescue -o $(BIN)/$(ISONAME).iso $(BOOTLOADER)

qemu: all
	@qemu-system-x86_64 $(QEMU_FLAGS) -cdrom $(BIN)/$(ISONAME).iso

check-multiboot2: all
	@if grub-file --is-x86-multiboot2 $(BOOTLOADER)/$(ISONAME).bin; then \
		echo multiboot confirmed; \
	else \
		echo the kernel is not multiboot2 compliant :\(; \
	fi

clean:
	@rm -rf $(BIN)
	@rm -f *.iso