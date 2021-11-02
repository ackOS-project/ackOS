#include "kernel/arch/arch.h"
#include "kernel/sys/panic.h"

#include <cstdio>

ATTRIBUTE_NO_RETURN void kpanic(const char* error, bool print_stacktrace, const std::source_location& location)
{
    arch::interrupts_disable();

    printf("kernel panic!\n"
            "    occurred in file %s:%d:%d in the function '%s()'\n"
            "    %s\n\n",
    location.file_name(), location.line(), location.column(), location.function_name(), error);

    if(print_stacktrace) arch::stacktrace_dump();

    while(true);
}
