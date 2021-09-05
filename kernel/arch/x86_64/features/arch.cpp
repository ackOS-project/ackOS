#include "kernel/arch/x86_64/features/instructions.h"
#include "kernel/arch/x86_64/features/com.h"
#include "kernel/arch/x86_64/features/paging.h"

#include "kernel/arch/arch.h"

#include <cstring>

namespace arch
{
    void interrupts_enable()
    {
        x86_64::interrupts_enable();
    }

    void interrupts_disable()
    {
        x86_64::interrupts_disable();
    }

    void halt()
    {
        x86_64::halt();
    }

    void early_print(const char* s)
    {
        for(int i = 0; i < strlen(s); i++)
        {
            x86_64::com_putc(x86_64::COM1, s[i]);
        }
    }

    void early_print_char(char c)
    {
        x86_64::com_putc(x86_64::COM1, c);
    }

    char early_getchar()
    {
        return x86_64::com_getc(x86_64::COM1);
    }

    uint64_t get_page_size()
    {
        return x86_64::paging_get_page_size();
    }
}