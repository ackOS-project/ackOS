#pragma once

#include <cstdint>

namespace x86_64
{
    void interrupts_enable();

    void interrupts_disable();

    void halt();

    uint64_t get_cr0();
#if 0
    // CR1 is reserved
    uint64_t get_cr1();
#endif
    uint64_t get_cr2();
    uint64_t get_cr3();
    uint64_t get_cr4();

    void set_cr0(uint64_t value);
#if 0
    void set_cr1(uint64_t value)
#endif
    void set_cr2(uint64_t value);
    void set_cr3(uint64_t value);
    void set_cr4(uint64_t value);
}
