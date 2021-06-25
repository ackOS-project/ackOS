#pragma once

#include <cstdint>
#include <cstddef>

enum
{
    VIRTUAL_FLAG_NONE = 0,
    VIRTUAL_FLAG_USER = (1 << 1),
    VIRTUAL_FLAG_CLEAR = (1 << 1)
};

void virtual_map(void* physical_addr, void* virtual_addr, uint32_t flags);
