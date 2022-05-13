#include "kernel/arch/x86_64/feat/msr.h"
#include "kernel/arch/x86_64/feat/cpuid.h"

namespace x86_64
{
    bool msr_check()
    {
        uint32_t eax, unused, edx;
        __get_cpuid(1, &eax, &unused, &unused, &edx);

        return edx & (uint32_t)x86_64::cpuid::FEAT_EDX_APIC;
    }

    void msr_get(uint32_t msr, uint32_t* low, uint32_t* high)
    {
        asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
    }

    void msr_set(uint32_t msr, uint32_t low, uint32_t high)
    {
        asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
    }
}