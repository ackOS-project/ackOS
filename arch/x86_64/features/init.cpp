#include "x86_64/features/CPUID.h"

void kmain();
extern "C" void enable_a20();
extern "C" void set_up_page_tables();
extern "C" void enable_paging();

extern "C" int x86_64_init()
{
    kmain();

    return 0;
}
