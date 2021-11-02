#include "kernel/arch/arch.h"
#include "kernel/mm/virtual.h"
#include "kernel/mm/memory.h"

#include "kernel/arch/x86_64/features/instructions.h"
#include <liback/utils/macros.h>

#define PAGE_SIZE (arch::get_page_size())

extern void* _kernel_start;
extern void* _kernel_end;

void virtual_initialise(uniheader* uheader)
{
    arch::paging_init();

    virtual_map(0, 0, memory_get_info()->total_memory, VIRTUAL_FLAG_WRITE);

    arch::paging_flush();
}

void virtual_map(uintptr_t virtual_mem, uintptr_t physical_mem, size_t size, uint32_t flags)
{
    for(uintptr_t i = 0; i < size; i += PAGE_SIZE)
    {
        arch::paging_map(virtual_mem + i, physical_mem + i, flags);
    }
}
