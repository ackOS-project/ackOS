#pragma once

#include <cstdint>
#include <cstddef>

// Architecture independent functions

namespace arch
{
    extern uint64_t page_size;

    void interrupts_enable();
    void interrupts_disable();

    void stacktrace_dump(void* addr);

    void halt();

    uintptr_t paging_create_table();
    void paging_load_table(uintptr_t table);
    void paging_map(uintptr_t table, uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags);
    void paging_unmap(uintptr_t table, uintptr_t virtual_addr);
    void paging_flush();

    void early_print(const char* s);
    void early_print_char(char c);
    size_t early_write(const char* s, size_t len);
    char early_getchar();
}
