#include "arch/x86_64/features/instructions.h"

namespace x86_64
{
    void interrupts_enable()
    {
        asm volatile("sti");
    }

    void interrupts_disable()
    {
        asm volatile("cli");
    }

    void halt()
    {
        asm volatile("hlt");
    }
}
