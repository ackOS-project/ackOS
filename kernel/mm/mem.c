#include "kernel/mm/mem.h"
#include "kernel/lib/log.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define NUM_OF_BITS 8

static struct {
    uint8_t* bitmap;
    size_t size;

    uintptr_t last_freed_page;

    size_t used_pages;
    size_t usable_pages;
    size_t reserved_pages;
} memory_pages = {0};

extern struct vmm_context kernel_vmm_context;

static inline size_t get_total_pages(void)
{
    return memory_pages.size * NUM_OF_BITS;
}

static inline size_t size_to_num_of_pages(size_t size)
{
    return DIV_UP(size, PAGE_SIZE);
}

static inline uintptr_t addr_to_page_index(phys_addr_t addr)
{
    if(addr % PAGE_SIZE != 0)
    {
        // address is unalligned

        return INVALID_PHYS_ADDR;
    }

    uintptr_t page_index = addr / PAGE_SIZE;

    if(page_index >= get_total_pages())
    {
        // address is out of range

        return INVALID_PHYS_ADDR;
    }

    return page_index;
}

static inline bool is_page_used(uintptr_t page_index)
{
    size_t index = page_index / NUM_OF_BITS;
    size_t bit_index = page_index % NUM_OF_BITS;

    return memory_pages.bitmap[index] & (1 << bit_index);
}

static inline void mark_page_as(uintptr_t page_index, bool value)
{
    size_t index = page_index / NUM_OF_BITS;
    size_t bit_index = page_index % NUM_OF_BITS;

    memory_pages.bitmap[index] = (memory_pages.bitmap[index] & ~(1 << bit_index)) | (value << bit_index);
}

static inline uintptr_t pmm_mark_pages(uintptr_t page_index, size_t page_count, bool state)
{
    for(uintptr_t i = page_index; i < page_index + page_count; i++)
    {
        if(is_page_used(i) == state)
        {
            return INVALID_PHYS_ADDR; /* error: already marked */
        }

        mark_page_as(i, state);
    }

    return page_index;
}

static inline uintptr_t pmm_mark_pages_and_tally(uintptr_t page_index, size_t page_count, bool state)
{
    if(pmm_mark_pages(page_index, page_count, state) == INVALID_PHYS_ADDR) return INVALID_PHYS_ADDR;

    if(state) /* reserve */
    {
        memory_pages.used_pages += page_count;
    }
    else /* free */
    {
        if(page_count > memory_pages.used_pages)
        {
            // tried to free more pages than have been allocated
            return INVALID_PHYS_ADDR;
        }

        memory_pages.used_pages -= page_count;
    }

    return page_index;
}

uintptr_t pmm_mark_addr(phys_addr_t addr, size_t size, bool state)
{
    return pmm_mark_pages(addr_to_page_index(addr), size_to_num_of_pages(size), state);
}

uintptr_t pmm_mark_addr_and_tally(phys_addr_t addr, size_t size, bool state)
{
    return pmm_mark_pages_and_tally(addr_to_page_index(addr), size_to_num_of_pages(size), state);
}

static bool pmm_can_store(uintptr_t page_index, size_t pages_needed)
{
    for(uintptr_t i = page_index; i < page_index + pages_needed; i++)
    {
        if(i >= get_total_pages() || is_page_used(i))
        {
            return false;
        }
    }

    return true;
}

uintptr_t pmm_find_pages(size_t pages_needed, uintptr_t start, uintptr_t end)
{
    for(uintptr_t i = start; i < end; i += pages_needed)
    {
        if(pmm_can_store(i, pages_needed))
        {
            return i;
        }
    }

    return INVALID_PHYS_ADDR;
}

phys_addr_t pmm_allocate(size_t size)
{
    size_t pages_needed = size_to_num_of_pages(size);
    uintptr_t page_index = pmm_find_pages(pages_needed, memory_pages.last_freed_page, get_total_pages());

    if(page_index == INVALID_PHYS_ADDR)
    {
        // start from the beginning
        page_index = pmm_find_pages(pages_needed, 1, memory_pages.last_freed_page);
    }

    if(page_index == INVALID_PHYS_ADDR)
    {
        kprintf(KERN_PANIC "Ran out of memory!\n");

        return INVALID_PHYS_ADDR;
    }

    pmm_mark_pages_and_tally(page_index, pages_needed, true);

    return page_index * PAGE_SIZE;
}

void pmm_deallocate(phys_addr_t addr, size_t size)
{
    if(addr == INVALID_PHYS_ADDR) return;

    memory_pages.last_freed_page = pmm_mark_addr_and_tally(addr, size, false);
}

void pmm_init(uint8_t* bitmap_memory, size_t size, size_t used_pages, size_t usable_pages, size_t reserved_pages)
{
    memory_pages.bitmap = bitmap_memory;
    memory_pages.size = size;
    memory_pages.last_freed_page = 0;
    memory_pages.used_pages = used_pages;
    memory_pages.usable_pages = usable_pages;
    memory_pages.reserved_pages = reserved_pages;

    // mark everything as unavailable
    memset(memory_pages.bitmap, 0xff, memory_pages.size);
}

struct vmm_context* get_kernel_vmm_context(void)
{
    return &kernel_vmm_context;
}

virt_addr_t mem_allocate_and_map(struct vmm_context* context, size_t size, uint32_t flags)
{
    phys_addr_t phys_addr = pmm_allocate(size);

    if (phys_addr == INVALID_PHYS_ADDR)
    {
        return NULL;
    }

    virt_addr_t virt_addr = vmm_allocate(context, phys_addr, size, flags);

    if (!virt_addr)
    {
        pmm_deallocate(phys_addr, size);

        return NULL;
    }

    return virt_addr;
}

bool mem_deallocate_and_unmap(struct vmm_context* context, virt_addr_t virt_addr, size_t size)
{
    phys_addr_t phys_addr = vmm_deallocate(context, virt_addr, size);

    if (phys_addr == INVALID_PHYS_ADDR)
    {
        return false;
    }

    pmm_deallocate(phys_addr, size);

    return true;
}

struct memory_info get_memory_info(void)
{
    struct memory_info info;

    info.used_bytes = memory_pages.used_pages * PAGE_SIZE;
    info.usable_bytes = memory_pages.usable_pages * PAGE_SIZE;
    info.reserved_bytes = memory_pages.reserved_pages * PAGE_SIZE;

    return info;
}

