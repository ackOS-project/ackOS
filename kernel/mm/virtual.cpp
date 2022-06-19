#include "kernel/arch/arch.h"
#include "kernel/mm/virtual.h"
#include "kernel/mm/memory.h"
#include "kernel/sys/panic.h"
#include "kernel/sys/logger.h"

#include "kernel/arch/x86_64/feat/paging.h"
#include "kernel/arch/x86_64/feat/asm.h"

#define PAGE_SIZE (arch::page_size)

extern void* _kernel_start;
extern void* _kernel_end;

static bool virtual_initialised = false;
static uintptr_t kernel_page_table = 0;

void virtual_initialise(uniheader* uheader)
{
    if(virtual_initialised)
    {
        kpanic("virtual memory management is already initialised");
    }

    uint32_t* fb = (uint32_t*)uheader->framebuffer.addr;

    kernel_page_table = arch::paging_create_table();

    for(int i = 0; i < uheader->memmap.entry_count; i++)
    {
        if(uheader->memmap.entries[i].type == UNIHEADER_MEMORY_USABLE)
        {
            virtual_map((uintptr_t)uheader->memmap.entries[i].addr, (uintptr_t)uheader->memmap.entries[i].addr, uheader->memmap.entries[i].length, VIRTUAL_FLAG_WRITE);
        }
    }

    virtual_map((uintptr_t)fb, (uintptr_t)fb, uheader->framebuffer.width * uheader->framebuffer.pitch, VIRTUAL_FLAG_WRITE);

    arch::paging_load_table(kernel_page_table);

    virtual_initialised = true;

    log_info("virtual", "vmm initialised");
}

void virtual_map(uintptr_t virtual_addr, uintptr_t physical_addr, size_t size, uint32_t flags)
{
    virtual_addr -= virtual_addr % PAGE_SIZE;
    physical_addr -= physical_addr % PAGE_SIZE;

    for(uintptr_t i = 0; i < size; i += PAGE_SIZE)
    {
        arch::paging_map(kernel_page_table, virtual_addr + i, physical_addr + i, flags);
    }

    if(virtual_initialised) arch::paging_flush();
}

void virtual_unmap(uintptr_t virtual_addr, size_t size)
{
    virtual_addr -= virtual_addr % PAGE_SIZE;

    for(uintptr_t i = 0; i < size; i += PAGE_SIZE)
    {
        arch::paging_unmap(kernel_page_table, virtual_addr + i);
    }

    if(virtual_initialised) arch::paging_flush();
}
