#include "kernel/mm/memory.h"
#include "kernel/mm/physical.h"
#include "kernel/mm/virtual.h"
#include "kernel/sys/logger.h"
#include "kernel/sys/panic.h"

#include <liback/utils/storage_size.h>
#include <cstdio>

static memory_info mem_info;
static bool memory_initialised = false;

void memory_initialise(uniheader* uheader)
{
    if(memory_initialised)
    {
        kpanic("memory management already initialised");
    }

    mem_info.used_memory = 0;
    mem_info.total_memory = uheader->get_usable_memory();

    physical_initialise(uheader);
    virtual_initialise(uheader);

    memory_initialised = true;
}

memory_info* memory_get_info()
{
    return &mem_info;
}

void memory_dump()
{
    log_info("memory", "Memory Usage: %s/%s", utils::format_storage_size(mem_info.used_memory).c_str(), utils::format_storage_size(mem_info.total_memory).c_str());
}
