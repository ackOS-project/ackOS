#pragma once

#include <cstddef>
#include <cstdint>

namespace x86_64
{
    bool msr_check();

    void msr_get(uint32_t msr, uint32_t* low, uint32_t *high);

    void msr_set(uint32_t msr, uint32_t low, uint32_t high);
}
