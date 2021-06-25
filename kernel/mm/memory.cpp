#include "kernel/mm/memory.h"
#include "kernel/mm/physical.h"

static memory_info mem_info;

void memory_initialise(uniheader* uheader)
{
    mem_info.total_memory = uheader->get_usable_memory();
    mem_info.used_memory = 0;

    physical_initialise(uheader);
}

memory_info* memory_get_info()
{
    return &mem_info;
}
