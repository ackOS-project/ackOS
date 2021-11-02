#pragma once

#include <cstdint>

// Architecture independent functions

namespace arch
{
    void interrupts_enable();
    void interrupts_disable();

    void stacktrace_dump();

    void halt();

    void paging_init();
    void paging_map(uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags);
    void paging_flush();
    uint64_t get_page_size();

    void early_print(const char* s);
    void early_print_char(char c);
    char early_getchar();
}
