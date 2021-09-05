#pragma once

#include "kernel/boot_protocols/uniheader.h"

#include <cstddef>
#include <cstdint>

struct memory_info
{
    uint64_t used_memory;
    uint64_t total_memory;
};

void memory_initialise(uniheader* uheader);

memory_info* memory_get_info();

void memory_dump();
