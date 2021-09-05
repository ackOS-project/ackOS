#pragma once

#include <cstdint>

namespace x86_64
{
    void interrupts_enable();

    void interrupts_disable();

    void halt();

    uint64_t get_cr0();
    uint64_t get_cr1();
    uint64_t get_cr2();
    uint64_t get_cr3();
    uint64_t get_cr4();
}
