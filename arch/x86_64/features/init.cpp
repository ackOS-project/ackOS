#include "x86_64/features/CPUID.h"
#include "x86_64/features/idt.h"

void kmain();

extern "C" int x86_64_init()
{
    kmain();

    return 0;
}
