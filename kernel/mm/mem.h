#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel/arch/common.h"

enum
{
    MAP_PAGE_WRITE = 1,
    MAP_PAGE_USER = (1 << 1),
    MAP_PAGE_EXECUTE = (1 << 2),
    MAP_LARGE_PAGES = (1 << 3), /* not yet implemented */
    MAP_NO_FLUSH = (1 << 4)
};

void vmm_init(struct vmm_context* context);
void vmm_deinit(struct vmm_context* context);

bool vmm_map(struct vmm_context* context, phys_addr_t phys, virt_addr_t virt, size_t size, uint32_t flags);
bool vmm_unmap(struct vmm_context* context, virt_addr_t virt, size_t size);

bool vmm_map_io(struct vmm_context* context, void(*read_func)(uint8_t* data, size_t offset, size_t size), void(*write_func)(const uint8_t* data, size_t offset, size_t size), virt_addr_t virt, size_t size, uint32_t flags);
// programme accesses memory at first page of virt
// this page is not mapped, so it triggers a page fault
// the page fault handler sees that this memory mapped IO
// so it allocates a physical page and calls the read_func to read in
// the data and then maps it to virt
// the instruction pointer is reset back to the code that triggered the page fault
// and the programme is now able to access the first page of virt
//ex2:
// programme writes byte into first page of virt
// this page is not mapped, so it triggers a page fault
// the page fault handler sees that this memory mapped IO
// so it allocates a physical page and calls the read_func to read in
// the data and then maps it to virt
// the instruction pointer is reset back to the code that triggered the page fault
// (some how modify the what comes directly after the code that triggered the page fault such that it calls write_func)
// and the programme is now able to write to the first page of virt

virt_addr_t vmm_allocate(struct vmm_context* context, phys_addr_t addr, size_t size, uint32_t flags);
phys_addr_t vmm_deallocate(struct vmm_context* context, virt_addr_t addr, size_t size);

virt_addr_t mem_allocate_and_map(struct vmm_context* context, size_t size, uint32_t flags);
bool mem_deallocate_and_unmap(struct vmm_context* context, virt_addr_t virt_addr, size_t size);

phys_addr_t vmm_virt_to_phys(const struct vmm_context* context, virt_addr_t virt);

void vmm_load(struct vmm_context* context);

struct vmm_context* get_kernel_vmm_context(void);

struct memory_info
{
    size_t used_bytes;
    size_t usable_bytes;
    size_t reserved_bytes;
};

struct memory_info get_memory_info(void);
