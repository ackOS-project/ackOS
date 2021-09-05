#include "kernel/mm/memory.h"
#include "kernel/mm/physical.h"

#include <liback/utils/bitmap.h>
#include <cstring>
#include <cstdio>

#define PAGE_SIZE 4096

static uintptr_t last_free = 0;
static utils::bitmap memory_bitmap{nullptr, 0};

void physical_initialise(uniheader* uheader)
{
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

    uint64_t bitmap_size = memory_get_info()->total_memory / 4096 / 8 + 1;

    memory_bitmap.reassign((uint8_t*)largest_free_mem_seg, bitmap_size);
    memset(largest_free_mem_seg, 0, bitmap_size);

    physical_lock(&memory_bitmap, memory_bitmap.get_size() / PAGE_SIZE + 1);
}

bool physical_page_is_free(void* addr)
{
    uintptr_t page = (uintptr_t)addr;

    return memory_bitmap.get(page) == false;
}

static uintptr_t physical_find_free(size_t size)
{
    return 0;
}

void* physical_allocate(size_t size)
{

    return nullptr;
}

void physical_lock_page(void* page)
{
    if(physical_page_is_free(page))
    {
        memory_get_info()->used_memory += PAGE_SIZE;

        memory_bitmap.set((uintptr_t)page, true);
    }
}

void physical_lock(void* page, size_t size)
{
    uintptr_t addr = (uintptr_t)page;

    for(int i = 0; i < size; i++)
    {
        physical_lock_page((void*)(addr + i));
    }
}

void physical_deallocate_page(void* page)
{
    uintptr_t addr = (uintptr_t)page;

    if(!physical_page_is_free((void*)addr))
    {
        memory_get_info()->used_memory -= PAGE_SIZE;

        memory_bitmap.set(addr, false);
    }

    last_free = (uintptr_t)page;
}

void physical_deallocate(void* addr, size_t size)
{
    uintptr_t page_addr = (uintptr_t)addr;

    for(int i = 0; i < size; i++)
    {
        physical_deallocate_page((void*)(page_addr + i));
    }
}
