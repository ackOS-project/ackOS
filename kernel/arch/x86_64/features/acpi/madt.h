#pragma once

#include <cstdint>
#include <cstddef>

namespace x86_64
{
    struct madt_info
    {
        uintptr_t lapic_addr;
        int cpu_cores;
    };

    madt_info madt_parse(void* rsdt);
}
