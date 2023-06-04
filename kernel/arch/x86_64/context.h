#pragma once

#include <stdint.h>
#include <stddef.h>

struct cpu_context
{
    void* vmm_table;
    void* vmm_last_free;
    size_t used_pages;
    size_t usable_pages;
    size_t reserved_pages;
};

struct cpu_context* obtain_kernel_context(void);
