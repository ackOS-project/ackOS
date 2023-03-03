#pragma once

#include "kernel/lib/util.h"

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE (4 KiB)

uintptr_t pmm_allocate(size_t size);

void pmm_deallocate(uintptr_t addr, size_t size);

void init_memory(void);
