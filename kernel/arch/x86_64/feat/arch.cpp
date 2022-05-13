#include "kernel/arch/x86_64/feat/asm.h"
#include "kernel/arch/x86_64/feat/com.h"
#include "kernel/arch/x86_64/feat/paging.h"
#include "kernel/arch/x86_64/feat/interrupts.h"

#include "kernel/arch/arch.h"

#include <cstring>

namespace arch
{
    uint64_t page_size = 4096;

    void interrupts_enable()
    {
        x86_64::interrupts_enable();
    }

    void interrupts_disable()
    {
        x86_64::interrupts_disable();
    }

    void stacktrace_dump(void* addr)
    {
        x86_64::dump_stackframe(addr);
    }

    void halt()
    {
        x86_64::halt();
    }

    void paging_init()
    {
        x86_64::paging_initialise();
    }

    void paging_map(uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags)
    {
        x86_64::paging_map(virtual_addr, phys_addr, flags);
    }

    void paging_unmap(uintptr_t virtual_addr)
    {
        x86_64::paging_unmap(virtual_addr);
    }

    void paging_flush()
    {
        x86_64::paging_flush();
    }

    void early_print(const char* s)
    {
        x86_64::com_write(x86_64::COM1, s, strlen(s));
    }

    void early_print_char(char c)
    {
        x86_64::com_putc(x86_64::COM1, c);
    }

    size_t early_write(const char* s, size_t len)
    {
        x86_64::com_write(x86_64::COM1, s, len);

        return len;
    }

    char early_getchar()
    {
        return x86_64::com_getc(x86_64::COM1);
    }
}