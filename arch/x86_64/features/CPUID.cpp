#include "x86_64/features/CPUID.h"
#include <cpuid.h>

int cpuid_get_model_id()
{
    int ebx, unused;
    __cpuid(0, unused, ebx, unused, unused);

    return ebx;
}
