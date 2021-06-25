#include "kernel/arch/x86_64/features/apic.h"
#include "kernel/arch/x86_64/features/msr.h"
#include "kernel/arch/x86_64/features/cpuid.h"

#include <cpuid.h>

static volatile uint32_t* lapic_addr;

namespace x86_64::apic
{
    uint32_t lapic_read(uint32_t reg)
    {
        return *((volatile uint32_t*)lapic_addr + reg);
    }

    void lapic_write(uint32_t reg, uint32_t data)
    {
        *((volatile uint32_t*)lapic_addr + reg) = data;
    }

    void initialise()
    {
        if(!check())
        {
            return;
        }


    }

    bool check()
    {
        uint32_t eax, unused, edx;
        __get_cpuid(1, &eax, &unused, &unused, &edx);

        return edx & x86_64::cpuid::FEAT_EDX_APIC;
    }
}
