#include "arch/x86_64/features/apic.h"
#include "arch/x86_64/features/CPUID.h"

#include <cpuid.h> // GCC header

bool apic_check()
{
    uint32_t eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);

    return edx & (uint32_t)cpuid_feature::EDX_APIC;
}
