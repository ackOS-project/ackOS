export PATH := $(shell toolchain/path.sh)

CC = x86_64-elf-gcc
CXX = x86_64-elf-g++
AS = nasm
LD = x86_64-elf-ld
BIN = bin

ISONAME = ackOS
BOOTLOADER = iso

SOURCES += \
		$(wildcard kernel/*.cpp)

HEADERS += \
		$(wildcard kernel/*.h) \
		$(wildcard lib/*.h) \
		$(wildcard lib/*/*.h) \

ASSEMBLY += \
		$(wildcard kernel/*.asm) \
		$(wildcard kernel/*/*.asm) \
		$(wildcard arch/*.asm)

FONTS += \
		$(wildcard fonts/*.psf)

OBJECTS = $(addprefix $(BIN)/, $(ASSEMBLY:.asm=.o)) $(addprefix $(BIN)/, $(SOURCES:.cpp=.o)) $(addprefix $(BIN)/, $(FONTS:.psf=.o))

CFLAGS += \
		-I lib/libc \
		-I lib/libstdc++ \
		-I lib \
		-I . \
		-ffreestanding \
		-fno-exceptions \
		-fno-rtti \
		-Ttext 0x8000 \
		-std=c++2a

LFLAGS +=

QEMU_FLAGS += \
			-enable-kvm \
			-serial stdio

$(BIN)/kernel/%.o: kernel/%.cpp $(HEADERS)
	@mkdir -p $(BIN)/kernel
	@echo [ compiling target $@ ] C++
	@$(CXX) -c $< -o $@ $(CFLAGS)

$(BIN)/arch/%.o: arch/%.asm
	@mkdir -p $(BIN)/arch
	@echo [ assemling target $@ ] Assembly
	@$(AS) -f elf64 $< -o $@

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