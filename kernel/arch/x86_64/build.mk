CFLAGS += -fno-stack-protector \
		  -fno-stack-check \
		  -fno-lto \
		  -fno-pie \
		  -fno-pic \
		  -m64 \
		  -march=x86-64 \
		  -mabi=sysv \
		  -mno-80387 \
		  -mno-mmx \
		  -mno-sse \
		  -mno-sse2 \
		  -mno-red-zone \
		  -mcmodel=kernel \
		  -MMD

LDFLAGS += -m elf_x86_64_$(SYSTEM) \
		   -z max-page-size=0x1000 \
		   -T kernel/arch/$(ARCH)/link.ld