#include "kernel/mm/memory.h"
#include "kernel/mm/physical.h"
#include "kernel/arch/arch.h"

#include <liback/utils/bitmap.h>
#include <cstring>

#define PAGE_SIZE (arch::page_size)

static uintptr_t last_free = 0;
static utils::bitmap memory_bitmap{nullptr, 0};

extern void* _kernel_start;
extern void* _kernel_end;

bool physical_page_is_free(void* addr)
{
    uintptr_t page = (uintptr_t)addr;

    return memory_bitmap.get(page) == false;
}

static bool physical_page_are_pages_free(void* start_addr, size_t size)
{
    for(uintptr_t i = 0; i < size; i++)
    {
        uintptr_t page = (uintptr_t)start_addr + (i * PAGE_SIZE);

        if(!physical_page_is_free((void*)page))
        {
            return false;
        }
    }

    return true;
}

static uintptr_t physical_find_free(size_t size)
{
    uintptr_t page = 0;

    if(physical_page_are_pages_free((void*)last_free, size))
    {
        page = last_free;
    }
    else
    {
        for(uintptr_t i = 0; i < memory_bitmap.get_size() * 8; i++)
        {
            if(physical_page_are_pages_free((void*)i, size))
            {
                page = i;

                break;
            }
        }
    }

    return page * PAGE_SIZE;
}

void* physical_allocate(size_t size)
{
    uintptr_t addr = physical_find_free(size);

    physical_lock((void*)addr, size);

    return (void*)addr;
}

void* physical_allocate_clear(size_t size)
{
    void* addr = physical_allocate(size);

    memset(addr, 0, size * PAGE_SIZE);

    return addr;
}

void physical_lock_page(void* page)
{
    uintptr_t addr = (uintptr_t)page / PAGE_SIZE;

    if(physical_page_is_free((void*)addr))
    {
        memory_get_info()->used_memory += PAGE_SIZE;

        memory_bitmap.set(addr, true);
    }
}

void physical_lock(void* page, size_t size)
{
    for(int i = 0; i < size; i++)
    {
        physical_lock_page((void*)((uintptr_t)page + (i * 4096)));
    }
}

static void physical_set_page_as(void* page, bool state)
{
    uintptr_t addr = (uintptr_t)page / PAGE_SIZE;

    memory_bitmap.set(addr, state);
}

static void physical_set_as(void* page, size_t size, bool state)
{
    for(int i = 0; i < size; i++)
    {
        physical_set_page_as((void*)((uintptr_t)page + (i * 4096)), state);
    }
}

void physical_deallocate_page(void* page)
{
    uintptr_t addr = (uintptr_t)page / PAGE_SIZE;

    if(!physical_page_is_free((void*)addr))
    {
        memory_get_info()->used_memory -= PAGE_SIZE;

        memory_bitmap.set(addr, false);
    }

    last_free = (uintptr_t)page;
}

void physical_deallocate(void* addr, size_t size)
{
    for(int i = 0; i < size; i++)
    {
        physical_deallocate_page((void*)((uintptr_t)addr + (i * PAGE_SIZE)));
    }
}

void physical_initialise(uniheader* uheader)
{
    uheader->dump_memmap();

    void* largest_free_mem_seg = nullptr;
    size_t largest_free_mem_seg_size = 0;

    for(int i = 0; i < uheader->memmap.entry_count; i++)
    {
        if(uheader->memmap.entries[i].type == UNIHEADER_MEMORY_USABLE)
        {
            if(uheader->memmap.entries[i].length > largest_free_mem_seg_size)
            {
                largest_free_mem_seg = uheader->memmap.entries[i].addr;
                largest_free_mem_seg_size = uheader->memmap.entries[i].length;
            }
        }
    }

    uint64_t bitmap_size = memory_get_info()->total_memory / PAGE_SIZE / 8 + 1;

    memory_bitmap.reassign((uint8_t*)largest_free_mem_seg, bitmap_size);
    memset(largest_free_mem_seg, 0xff, bitmap_size);
    
    for(int i = 0; i < uheader->memmap.entry_count; i++)
    {
        uniheader_memory_map_entry* entry = &uheader->memmap.entries[i];

        if(entry->type == UNIHEADER_MEMORY_USABLE)
        {
            physical_set_as(entry->addr, entry->length / PAGE_SIZE + 1, false);
        }
    }

    size_t kernel_size = ((uintptr_t)&_kernel_end - (uintptr_t)&_kernel_start) / PAGE_SIZE + 1;

    physical_lock(&_kernel_start, kernel_size);
    physical_set_as((void*)0, 1, true);
}
