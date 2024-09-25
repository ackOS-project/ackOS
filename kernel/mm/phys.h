#pragma once

#include <stddef.h>
#include <stdint.h>

#include "kernel/arch/common.h"

uintptr_t pmm_mark_addr(phys_addr_t addr, size_t size, bool state);
uintptr_t pmm_mark_addr_and_tally(phys_addr_t addr, size_t size, bool state);

phys_addr_t pmm_allocate(size_t size);
void pmm_deallocate(phys_addr_t addr, size_t size);