#ifdef BUILD_ARCH_x86_64
#include "arch/x86_64/features/instructions.h"
#endif

#include "arch.h"

namespace arch
{
    void interrupts_enable()
    {
        #ifdef BUILD_ARCH_x86_64
        x86_64::interrupts_enable();
        #endif
    }

    void interrupts_disable()
    {
        #ifdef BUILD_ARCH_x86_64
        x86_64::interrupts_disable();
        #endif
    }

    void halt()
    {
        #ifdef BUILD_ARCH_x86_64
        x86_64::halt();
        #endif
    }
}