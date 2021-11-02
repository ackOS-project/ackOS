#pragma once

#include "kernel/boot_protocols/uniheader.h"

#include <cstdint>
#include <cstddef>

enum virtual_flag_t
{
    VIRTUAL_FLAG_NONE,
    VIRTUAL_FLAG_WRITE,
    VIRTUAL_FLAG_USER
};

void virtual_initialise(uniheader* uheader);

void virtual_map(uintptr_t physical_addr, uintptr_t virtual_addr, size_t size, uint32_t flags);
