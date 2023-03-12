#pragma once

#include "kernel/lib/util.h"

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

phys_addr_t pmm_allocate(size_t size);
void pmm_deallocate(phys_addr_t addr, size_t size);

void init_memory(void);
