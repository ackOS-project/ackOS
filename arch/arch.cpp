#ifdef BUILD_ARCH_x86_64
#include "x86_64/features/interrupts.h"
#endif

#include "arch.h"

void arch_interrupts_enable()
{
    #ifdef BUILD_ARCH_x86_64
    interrupts_enable();
    #endif
}

void arch_interrupts_disable()
{
    #ifdef BUILD_ARCH_x86_64
    interrupts_disable();
    #endif
}
