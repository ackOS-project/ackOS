#include "kernel/arch/arch.h"
#include "kernel/sys/panic.h"
#include "kernel/sys/logger.h"

ATTRIBUTE_NO_RETURN void kpanic(const char* error, void* stacktrace_addr, const std::source_location& location)
{
    arch::interrupts_disable();

    log_error("panic", error);
    log_error("panic", "panicked in file %s:%d:%d in the function '%s()'",
            location.file_name(), location.line(), location.column(), location.function_name());

    arch::stacktrace_dump(stacktrace_addr);

    while(true)
    {
        arch::halt();
    }
}
