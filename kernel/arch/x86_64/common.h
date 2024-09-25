#pragma once

#include <liback/util.h>

#define PAGE_SIZE (4 KiB)
#define INVALID_PHYS_ADDR (phys_addr_t)(1UL << 52)

typedef uintptr_t phys_addr_t;
typedef void* virt_addr_t;

struct vmm_context
{
    void* vmm_table;
};
