#include "kernel/arch/arch.h"
#include "kernel/mm/virtual.h"
#include "kernel/mm/memory.h"
#include "kernel/sys/panic.h"

#define PAGE_SIZE (arch::page_size)

extern void* _kernel_start;
extern void* _kernel_end;

static bool virtual_initialised = false;

void virtual_initialise(uniheader* uheader)
{
    if(virtual_initialised)
    {
        kpanic("virtual memory management is already initialised");
    }

    arch::paging_init();

    virtual_map(0, 0, memory_get_info()->total_memory, VIRTUAL_FLAG_WRITE);

    for(int i = 0; i < uheader->memmap.entry_count; i++)
    {
        if(uheader->memmap.entries[i].type == UNIHEADER_MEMORY_USABLE)
        {
            virtual_map((uintptr_t)uheader->memmap.entries[i].addr, (uintptr_t)uheader->memmap.entries[i].addr, uheader->memmap.entries[i].length, VIRTUAL_FLAG_WRITE);
        }
    }

    arch::paging_flush();

    virtual_initialised = true;
}

void virtual_map(uintptr_t virtual_addr, uintptr_t physical_addr, size_t size, uint32_t flags)
{
    for(uintptr_t i = 0; i < size; i += PAGE_SIZE)
    {
        arch::paging_map(virtual_addr + i, physical_addr + i, flags);
    }

    if(virtual_initialised) arch::paging_flush();
}

void virtual_unmap(uintptr_t virtual_addr, size_t size)
{
    for(uintptr_t i = 0; i < size; i += PAGE_SIZE)
    {
        arch::paging_unmap(virtual_addr + i);
    }

    if(virtual_initialised) arch::paging_flush();
}
