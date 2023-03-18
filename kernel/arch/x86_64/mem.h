#pragma once

#include "kernel/arch/x86_64/context.h"
#include <liback/util.h>

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE (4 KiB)

enum
{
    MAP_PAGE_WRITE = 1,
    MAP_PAGE_USER = (1 << 1),
    MAP_PAGE_EXECUTE = (1 << 2),
    MAP_PAGE_NO_FLUSH = (1 << 3)
};

typedef uintptr_t phys_addr_t;
typedef void* virt_addr_t;

phys_addr_t pmm_allocate(struct cpu_context* context, size_t size);
void pmm_deallocate(struct cpu_context* context, phys_addr_t addr, size_t size);

void vmm_init(struct cpu_context* context);
void vmm_init_for_kernel_space(struct cpu_context* context);
void vmm_deinit(struct cpu_context* context);
void vmm_load(struct cpu_context* context);
bool vmm_map(struct cpu_context* context, phys_addr_t phys, virt_addr_t virt, size_t size, uint32_t flags);
bool vmm_unmap(struct cpu_context* context, virt_addr_t virt, size_t size);

void vmm_print_mapping(struct cpu_context* context, virt_addr_t virt);

void init_memory(void);
