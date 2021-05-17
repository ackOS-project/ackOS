#include <cstdio>
#include "arch/arch.h"
#include "kernel/panic.h"

void kpanic(const char* error, const std::source_location& location)
{
    arch_interrupts_disable();

    printf("kernel panic!\n"
            "    occurred in file %s:%d:%d in the function '%s()'\n"
            "    %s\n",
    location.file_name(), location.line(), location.column(), location.function_name(), error);

    while(true);
}
