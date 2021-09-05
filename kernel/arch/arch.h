#pragma once

#include <cstdio>
#include <cstdint>

// Architecture independent functions

namespace arch
{
    void interrupts_enable();
    void interrupts_disable();

    void halt();

    void early_print(const char* s);
    void early_print_char(char c);
    char early_getchar();

    uint64_t get_page_size();
}
