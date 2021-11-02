#include "kernel/arch/x86_64/features/acpi/apic.h"
#include "kernel/arch/x86_64/features/msr.h"
#include "kernel/arch/x86_64/features/cpuid.h"
#include "kernel/arch/arch.h"

static volatile uint32_t* lapic_addr = nullptr;

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
            arch::early_print("warning: APIC support was not detected\n");

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
