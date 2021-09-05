#include "kernel/mm/memory.h"
#include "kernel/mm/physical.h"
#include "kernel/logger.h"

#include <liback/utils/storage_size.h>

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

void memory_dump()
{
    std::string used_mem;
    used_mem = utils::format_storage_size(mem_info.used_memory);
    std::string total_mem;
    total_mem = utils::format_storage_size(mem_info.total_memory);

    log_info("memory", "Memory Usage: %s/%s", used_mem.c_str(), total_mem.c_str());
}
