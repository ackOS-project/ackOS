#pragma once

#include "kernel/arch/x86_64/context.h"
#include <liback/util.h>

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE (4 KiB)
#define INVALID_PHYS (1UL << 52)

enum
{
    MAP_PAGE_WRITE = 1,
    MAP_PAGE_USER = (1 << 1),
    MAP_PAGE_EXECUTE = (1 << 2),
    MAP_LARGE_PAGES = (1 << 3), /* not yet implemented */
    MAP_NO_FLUSH = (1 << 4)
};

typedef uintptr_t phys_addr_t;
typedef void* virt_addr_t;

// a direct map of up to 4GiB of physical memory
virt_addr_t physical_to_io(phys_addr_t addr);
phys_addr_t io_to_physical(virt_addr_t addr);

phys_addr_t pmm_allocate(struct cpu_context* context, size_t size);
void pmm_deallocate(struct cpu_context* context, phys_addr_t addr, size_t size);

void vmm_init(struct cpu_context* context);
void vmm_init_for_kernel_space(struct cpu_context* context);
void vmm_deinit(struct cpu_context* context);
void vmm_load(struct cpu_context* context);
bool vmm_map(struct cpu_context* context, phys_addr_t phys, virt_addr_t virt, size_t size, uint32_t flags);
bool vmm_unmap(struct cpu_context* context, virt_addr_t virt, size_t size);
phys_addr_t vmm_virt_to_phys(struct cpu_context* context, virt_addr_t virt);

void vmm_print_mapping(struct cpu_context* context, virt_addr_t virt);

void init_memory(void);
