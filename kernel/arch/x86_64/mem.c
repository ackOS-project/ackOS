#include <limine/limine.h>
#include <string.h>

#include "kernel/arch/x86_64/mem.h"
#include "kernel/lib/log.h"

static volatile struct limine_memmap_request memmap_request =
{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

static volatile struct limine_hhdm_request hhdm_request =
{
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static struct {
    uint8_t* bitmap;
    size_t size;
    size_t used_pages;
    size_t usable_pages;
    size_t reserved_pages;

    uintptr_t last_freed_page;
} memory_pages = {0};

#define NUM_OF_BITS 8

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

static inline size_t size_to_num_of_pages(size_t size)
{
    return DIV_UP(size, PAGE_SIZE);
}

static inline uintptr_t get_total_pages(void)
{
    return memory_pages.size * NUM_OF_BITS;
}

static inline uintptr_t addr_to_page_index(uintptr_t addr)
{
    if(addr % PAGE_SIZE != 0)
    {
        kprintf(KERN_PANIC "addr_to_page_index: address %#lx is unalligned (needs to be %'zu alligned)\n", addr, (size_t)PAGE_SIZE);
    }

    uintptr_t page_index = addr / PAGE_SIZE;

    if(page_index >= get_total_pages())
    {
        kprintf(KERN_PANIC "addr_to_page_index: address %#lx is out of range\n", addr);
    }

    return page_index;
}

static inline void pmm_mark_pages(uintptr_t page_index, size_t page_count, bool state)
{
    for(size_t i = page_index; i < page_index + page_count; i++)
    {
        if(is_page_used(i) == state)
        {
            kprintf(KERN_PANIC "pmm_mark_pages: attempted to mark already marked page\n");

            return;
        }

        mark_page_as(i, state);
    }
}

static inline uintptr_t pmm_mark_pages_and_tally(uintptr_t page_index, size_t page_count, bool state)
{
    pmm_mark_pages(page_index, page_count, state);

    if(state) /* reserve */
    {
        memory_pages.used_pages += page_count;
    }
    else /* free */
    {
        if(page_count > memory_pages.used_pages)
        {
            kprintf(KERN_PANIC "pmm_mark_pages_and_tally: trying to free more pages than have been allocated\n");

            return 0;
        }

        memory_pages.used_pages -= page_count;
    }

    return page_index;
}

static inline void pmm_mark_addr(uintptr_t addr, size_t size, bool state)
{
    pmm_mark_pages(addr_to_page_index(addr), size_to_num_of_pages(size), state);
}

static inline uintptr_t pmm_mark_addr_and_tally(uintptr_t addr, size_t size, bool state)
{
    uintptr_t page_index = addr_to_page_index(addr);
    size_t page_count = size_to_num_of_pages(size);
    
    return pmm_mark_pages_and_tally(page_index, page_count, state);
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

    return 0;
}

uintptr_t pmm_allocate(size_t size)
{
    size_t pages_needed = size_to_num_of_pages(size);
    uintptr_t page_index = pmm_find_pages(pages_needed, memory_pages.last_freed_page, get_total_pages());

    if(!page_index)
    {
        // start from the beginning
        page_index = pmm_find_pages(pages_needed, 1, memory_pages.last_freed_page);
    }

    if(!page_index)
    {
        kprintf(KERN_PANIC "Ran out of memory!\n");

        return 0;
    }

    pmm_mark_pages_and_tally(page_index, pages_needed, true);

    return page_index * PAGE_SIZE;
}

void pmm_deallocate(uintptr_t addr, size_t size)
{
    memory_pages.last_freed_page = pmm_mark_addr_and_tally(addr, size, false);
}

const char* get_limine_mem_map_type_as_string(uint64_t type)
{
    switch(type)
    {
        case LIMINE_MEMMAP_USABLE:
            return "Usable";
        case LIMINE_MEMMAP_RESERVED:
            return "Reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI Reclaimable";
        case LIMINE_MEMMAP_ACPI_NVS:
            return "ACPI NVS";
        case LIMINE_MEMMAP_BAD_MEMORY:
            return "Bad Memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader Reclaimable";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            return "Kernel and Modules";
        case LIMINE_MEMMAP_FRAMEBUFFER:
            return "Framebuffer";
        default:
            return "(Unknown)";
    }
}

void init_memory()
{
    if(memmap_request.response == NULL)
    {
        kprintf(KERN_PANIC "response to `limine_memmap_request` was null\n");

        return;
    }
    else if(memmap_request.response->entry_count == 0 || memmap_request.response->entries == NULL)
    {
        kprintf(KERN_PANIC "limine memory map is empty\n");

        return;
    }

    if(hhdm_request.response == NULL)
    {
        kprintf(KERN_PANIC "response to `limine_hhdm_request` was null\n");

        return;
    }

    size_t total_required = 0;

    kprintf(KERN_DEBUG "Memory regions:\n");

    // Calculate the memory available
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        kprintf(KERN_DEBUG "    %-25s %#lx (%zu)\n", get_limine_mem_map_type_as_string(entry->type), entry->base, entry->length);

        total_required += entry->length;

        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            memory_pages.usable_pages += entry->length / PAGE_SIZE;
        }
        else
        {
            memory_pages.reserved_pages += entry->length / PAGE_SIZE;
        }
    }

    total_required = DIV_UP(total_required, PAGE_SIZE);
    total_required = DIV_UP(total_required, NUM_OF_BITS);

    struct limine_memmap_entry* suitable_entry = NULL;

    // Find the smallest available memory region to fit the bitmap
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type != LIMINE_MEMMAP_USABLE || entry->length < total_required) continue;

        if(entry->length == total_required)
        {
            // perfect
            suitable_entry = entry;

            break;
        }
        else if(!suitable_entry || entry->length < suitable_entry->length)
        {
            suitable_entry = entry;
        }
    }

    if(!suitable_entry)
    {
        kprintf(KERN_PANIC "Could not find memory region to place bitmap at!\n");

        return;
    }

    if(suitable_entry->length < total_required)
    {
        kprintf(KERN_PANIC "Could not find an adequate memory region to place bitmap at! needed %'zu and could only find %'zu\n",
                            total_required,
                            suitable_entry->length);

        return;
    }

    memory_pages.bitmap = (uint8_t*)(hhdm_request.response->offset + suitable_entry->base);
    memory_pages.size = total_required;

    // mark everything as unavailable
    memset(memory_pages.bitmap, 0xff, memory_pages.size);

    // just kidding,
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            pmm_mark_addr(entry->base, entry->length, false);
        }
    }

    // reserve bitmap
    pmm_mark_addr(suitable_entry->base, total_required, true);

    kprintf(KERN_DEBUG "Storing memory bitmap at %p\n", memory_pages.bitmap);
    kprintf(KERN_INFO "Used memory:     %'zu\n"
                      "Usable memory:   %'zu\n"
                      "Reserved memory: %'zu\n",
                      memory_pages.used_pages * PAGE_SIZE,
                      memory_pages.usable_pages * PAGE_SIZE,
                      memory_pages.reserved_pages * PAGE_SIZE);   
}
