#include "x86_64/features/CPUID.h"

void kmain();
extern "C" void enable_a20();

extern "C" int x86_64_init()
{
    asm("cli"); // Disable interupts
    enable_a20();

    kmain();

    return 0;
}
