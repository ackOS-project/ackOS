#pragma once

#include "kernel/arch/x86_64/common.h"

#include <stdint.h>
#include <stddef.h>

// a direct map of up to 4GiB of physical memory
virt_addr_t physical_to_io(phys_addr_t addr);
phys_addr_t io_to_physical(virt_addr_t addr);

void vmm_init_for_kernel_space(struct vmm_context* context);
void vmm_print_mapping(const struct vmm_context* context, virt_addr_t virt);

void init_memory(void);
