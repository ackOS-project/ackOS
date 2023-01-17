# The wildcard grabs all architectures, we only want one, so we have to filter them out.
KERNEL_C += $(filter-out $(call rwildcard,kernel/arch,*.c), $(call rwildcard,kernel,*.c)) \
		   $(call rwildcard,kernel/arch/$(ARCH),*.c)

KERNEL_S += $(filter-out $(call rwildcard,kernel/arch,*.s), $(call rwildcard,kernel,*.s)) \
		   $(call rwildcard,kernel/arch/$(ARCH),*.s)

KERNEL_TARGETS += $(KERNEL_C:%.c=$(OBJ_DIR)/%.c.o) $(KERNEL_S:%.s=$(OBJ_DIR)/%.s.o)

KERNEL_BIN := $(BIN_DIR)/kernel.elf

CFLAGS += -I . -I submodules -I kernel/lib/libc -D KERNEL_DEBUG

include kernel/arch/$(ARCH)/build.mk

$(KERNEL_BIN): $(KERNEL_TARGETS)
	$(CREATE_DIRS)
	@echo "Linking kernel"
	$(V)$(LD) $(LDFLAGS) -o $@ $(KERNEL_TARGETS)
