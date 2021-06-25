KERNEL_SOURCES += \
		$(call rwildcard,kernel,*.cpp)
KERNEL_SOURCES := $(filter-out $(call rwildcard,kernel/arch,*.cpp), $(KERNEL_SOURCES))
KERNEL_SOURCES += \
		$(call rwildcard,kernel/arch/$(ARCH),*.cpp) \
		$(wildcard kernel/arch/*.cpp)

KERNEL_HEADERS += \
		$(wildcard kernel/*.h) \
		$(wildcard lib/*.h) \
		$(wildcard lib/*/*.h)

KERNEL_ASSEMBLY += \
		$(call rwildcard,kernel/arch/$(ARCH),*.asm)
KERNEL_ASSEMBLY := $(filter-out $(call rwildcard,kernel/arch,*.asm), $(KERNEL_ASSEMBLY))
KERNEL_ASSEMBLY += \
		$(call rwildcard,kernel/arch/$(ARCH),*.asm) \
		$(wildcard kernel/arch/*.asm)

KERNEL_OBJECTS := $(KERNEL_ASSEMBLY:%.asm=$(BIN_FOLDER)/%.asm.o) $(KERNEL_SOURCES:%.cpp=$(BIN_FOLDER)/%.o) $(FONTS:%.psf=$(BIN_FOLDER)/%.o)
