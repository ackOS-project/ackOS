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
    uheader->memmap.align(PAGE_SIZE);

    // memset((void*)memory_bitmap_base, 0xff, MEMORY_BITMAP_SIZE / 8);
}

bool physical_page_is_free(void* addr)
{
    uintptr_t page = (uintptr_t)addr * PAGE_SIZE;

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

void physical_deallocate(void* addr, size_t size)
{
    for(int i = 0; i < size; i++)
    {
        uintptr_t page = (uintptr_t)addr + i;

        last_free = page;

        if(!physical_page_is_free((void*)page))
        {
            memory_get_info()->used_memory -= PAGE_SIZE;

            memory_bitmap.set(page, false);
        }
    }
}
