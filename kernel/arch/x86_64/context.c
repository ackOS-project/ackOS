#include "kernel/arch/x86_64/context.h"

static struct cpu_context kernel_context = {0};

struct cpu_context* obtain_kernel_context(void)
{
    return &kernel_context;
}
