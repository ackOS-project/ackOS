#pragma once

#include <cstdint>

// Architecture independent functions

namespace arch
{
    extern uint64_t page_size;

    void interrupts_enable();
    void interrupts_disable();

    void stacktrace_dump();

    void halt();

    void paging_init();
    void paging_map(uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags);
    void paging_unmap(uintptr_t virtual_addr);
    void paging_flush();

    void early_print(const char* s);
    void early_print_char(char c);
    char early_getchar();
}
