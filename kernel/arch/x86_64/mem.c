#include <limine/limine.h>
#include <string.h>

#include "kernel/arch/x86_64/mem.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/cpuid.h"
#include "kernel/lib/log.h"

// enable five level paging if available
static volatile struct limine_5_level_paging_request five_level_request =
{
    .id = LIMINE_5_LEVEL_PAGING_REQUEST,
    .revision = 0
};

static volatile struct limine_memmap_request memmap_request =
{
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

volatile struct limine_hhdm_request hhdm_request =
{
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request kern_addr_request =
{
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

static struct {
    uint8_t* bitmap;
    size_t size;

    uintptr_t last_freed_page;
} memory_pages = {0};

#define INVALID_PHYS (1L << 52)
#define NUM_OF_BITS 8

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

        return INVALID_PHYS;
    }

    uintptr_t page_index = addr / PAGE_SIZE;

    if(page_index >= get_total_pages())
    {
        // address is out of range

        return INVALID_PHYS;
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
            return INVALID_PHYS; /* error: already marked */
        }

        mark_page_as(i, state);
    }

    return page_index;
}

static inline uintptr_t pmm_mark_pages_and_tally(struct cpu_context* context, uintptr_t page_index, size_t page_count, bool state)
{
    if(pmm_mark_pages(page_index, page_count, state) == INVALID_PHYS) return INVALID_PHYS;

    if(state) /* reserve */
    {
        context->used_pages += page_count;
    }
    else /* free */
    {
        if(page_count > context->used_pages)
        {
            // tried to free more pages than have been allocated
            return INVALID_PHYS;
        }

        context->used_pages -= page_count;
    }

    return page_index;
}

static inline uintptr_t pmm_mark_addr(phys_addr_t addr, size_t size, bool state)
{
    return pmm_mark_pages(addr_to_page_index(addr), size_to_num_of_pages(size), state);
}

static inline uintptr_t pmm_mark_addr_and_tally(struct cpu_context* context, phys_addr_t addr, size_t size, bool state)
{
    return pmm_mark_pages_and_tally(context, addr_to_page_index(addr), size_to_num_of_pages(size), state);
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

    return INVALID_PHYS;
}

phys_addr_t pmm_allocate(struct cpu_context* context, size_t size)
{
    size_t pages_needed = size_to_num_of_pages(size);
    uintptr_t page_index = pmm_find_pages(pages_needed, memory_pages.last_freed_page, get_total_pages());

    if(page_index == INVALID_PHYS)
    {
        // start from the beginning
        page_index = pmm_find_pages(pages_needed, 1, memory_pages.last_freed_page);
    }

    if(page_index == INVALID_PHYS)
    {
        kprintf(KERN_PANIC "Ran out of memory!\n");

        return INVALID_PHYS;
    }

    pmm_mark_pages_and_tally(context, page_index, pages_needed, true);

    return page_index * PAGE_SIZE;
}

void pmm_deallocate(struct cpu_context* context, phys_addr_t addr, size_t size)
{
    memory_pages.last_freed_page = pmm_mark_addr_and_tally(context, addr, size, false);
}

static inline virt_addr_t physical_to_logical(phys_addr_t addr)
{
    return (virt_addr_t)(hhdm_request.response->offset + addr);
}

static inline phys_addr_t logical_to_physical(virt_addr_t addr)
{
    return (phys_addr_t)(addr - hhdm_request.response->offset);
}

virt_addr_t pmm_allocate_logical_addr(struct cpu_context* context, size_t size)
{
    phys_addr_t addr = pmm_allocate(context, size);

    if(addr == INVALID_PHYS) return NULL;

    return physical_to_logical(addr);
}

virt_addr_t pmm_allocate_zeroed_logical_addr(struct cpu_context* context, size_t size)
{
    virt_addr_t addr = pmm_allocate_logical_addr(context, size);

    if(addr != NULL)
    {
        memset(addr, 0, size);
    }

    return addr;
}

void pmm_deallocate_logical_addr(struct cpu_context* context, virt_addr_t addr, size_t size)
{
    pmm_deallocate(context, logical_to_physical(addr), size);
}

// VMM
#define L5_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 48)) >> 48)
#define L4_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 39)) >> 39)
#define L3_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 30)) >> 30)
#define L2_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 21)) >> 21)
#define L1_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 12)) >> 12)

struct ATTR_PACKED level_entry
{
    uint64_t present : 1,
             read_write : 1,
             user_supervisor : 1,
             write_through : 1,
             cache_disable : 1,
             accessed : 1,
             dirty : 1,
             huge_pages : 1,
             global : 1,
             : 3,
             phys_page_index : 40,
             reserved : 11,
             execute_disable : 1;
};

struct ATTR_PACKED paging_table
{
    struct level_entry entries[512];
};

#define LEVEL_TABLE_ENTRY_COUNT(TABLE_) (sizeof((TABLE_)->entries) / sizeof((TABLE_)->entries[0]))

_Static_assert(sizeof(struct paging_table) == PAGE_SIZE);

static inline struct level_entry vmm_create_level_entry(phys_addr_t phys_addr, uint32_t flags)
{
    struct level_entry entry = {0};
    
    entry.present = true;
    entry.phys_page_index = phys_addr / PAGE_SIZE;
    entry.read_write = (flags & MAP_PAGE_WRITE) != 0;
    entry.user_supervisor = (flags & MAP_PAGE_USER) != 0;
    entry.write_through = false;
    entry.cache_disable = false;
    entry.accessed = false;
    entry.dirty = false;
    entry.huge_pages = false;
    entry.global = false;
    entry.execute_disable = (flags & MAP_PAGE_EXECUTE) == 0;

    return entry;
}

static inline struct level_entry vmm_create_null_level_entry(void)
{
    struct level_entry entry = {0};

    return entry;
}

static struct paging_table* vmm_create_table(struct cpu_context* context)
{
    return (struct paging_table*)pmm_allocate_zeroed_logical_addr(context, PAGE_SIZE);
}

static inline void vmm_destroy_level(struct cpu_context* context, struct paging_table* level)
{
    pmm_deallocate_logical_addr(context, level, PAGE_SIZE);
}

void vmm_init(struct cpu_context* context)
{
    context->vmm_table = (void*)vmm_create_table(context);
}

static struct paging_table* vmm_next_table(struct paging_table* table, size_t index)
{
    if(!table || index >= (sizeof(table->entries) / sizeof(table->entries[0])))
    {
        kprintf(KERN_PANIC "vmm_next_table: invalid index or table\n");
    }

    if(table->entries[index].present)
    {
        return physical_to_logical(table->entries[index].phys_page_index * PAGE_SIZE);
    }
    else
    {
        kprintf(KERN_PANIC "vmm_next_table: level entry is not present\n");
    }

    return NULL;
}

static struct paging_table* vmm_next_table_or_alloc(struct cpu_context* context, struct paging_table* table, size_t index, uint32_t flags)
{
    struct paging_table* next_table = NULL;

    if(!table || index >= LEVEL_TABLE_ENTRY_COUNT(table))
    {
        kprintf(KERN_PANIC "vmm_next_table_or_alloc: invalid index or table\n");

        return NULL;
    }

    if(table->entries[index].present)
    {
        next_table = physical_to_logical(table->entries[index].phys_page_index * PAGE_SIZE);
    }
    else
    {
        next_table = vmm_create_table(context);
        table->entries[index] = vmm_create_level_entry(logical_to_physical(next_table), flags);
    }

    return next_table;
}

bool vmm_map_page(struct cpu_context* context, phys_addr_t phys, virt_addr_t virt, uint32_t flags)
{
    struct paging_table* table = context->vmm_table;
    struct paging_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        l4 = vmm_next_table_or_alloc(context, table, L5_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }

    struct paging_table* l3 = vmm_next_table_or_alloc(context, l4, L4_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    struct paging_table* l2 = vmm_next_table_or_alloc(context, l3, L3_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    struct paging_table* l1 = vmm_next_table_or_alloc(context, l2, L2_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);

    struct level_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        *entry = vmm_create_level_entry(phys, flags);
    }
    else
    {
        // page was present
        return false;
    }

    if(!(flags & MAP_PAGE_NO_FLUSH) && reg_get_cr3() == logical_to_physical(table))
    {
        invalidate_page(virt);
    }

    return true;
}

bool vmm_map(struct cpu_context* context, phys_addr_t phys, virt_addr_t virt, size_t size, uint32_t flags)
{
    for(size_t off = 0; off < size; off += PAGE_SIZE)
    {
        if(!vmm_map_page(context, phys + off, (virt_addr_t)(((uintptr_t)virt) + off), flags))
        {
            // TODO: undo previous mappings

            return false;
        }
    }

    return true;
}

static void vmm_free_table_if_needed(struct cpu_context* context, struct paging_table* parent, size_t index)
{
    struct paging_table* table = physical_to_logical(parent->entries[index].phys_page_index * PAGE_SIZE);

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(table); i++)
    {
        if(table->entries[i].present)
        {
            return;
        }
    }

    vmm_destroy_level(context, table);

    parent->entries[index] = vmm_create_null_level_entry();
}

bool vmm_unmap_page(struct cpu_context* context, virt_addr_t virt)
{
    struct paging_table* table = context->vmm_table;

    if(!table) return false;
    
    struct paging_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        l4 = vmm_next_table(table, L5_INDEX((uintptr_t)virt));

        if(!l4) return false;
    }

    struct paging_table* l3 = vmm_next_table(l4, L4_INDEX((uintptr_t)virt));
    if(!l3) return false;

    struct paging_table* l2 = vmm_next_table(l3, L3_INDEX((uintptr_t)virt));
    if(!l2) return false;

    struct paging_table* l1 = vmm_next_table(l2, L2_INDEX((uintptr_t)virt));
    if(!l1) return false;

    struct level_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        // not mapped
        return false;
    }

    *entry = vmm_create_null_level_entry();

    // we should free the levels if needed
    // but not the top level
    vmm_free_table_if_needed(context, l2, L2_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(context, l3, L3_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(context, l4, L4_INDEX((uintptr_t)virt));

    if(five_level_request.response != NULL)
    {
        vmm_free_table_if_needed(context, table, L5_INDEX((uintptr_t)virt));
    }

    return true;
}

bool vmm_unmap(struct cpu_context* context, virt_addr_t virt, size_t size)
{
    for(size_t off = 0; off < size; off += PAGE_SIZE)
    {
        if(vmm_unmap_page(context, (virt_addr_t)(((uintptr_t)virt) + off)))
        {
            // TODO: undo previous unmappings

            return false;
        }
    }

    return true;
}

static void vmm_print_level_entry(const struct paging_table* table, size_t index)
{
    const struct level_entry* entry = &table->entries[index];

    kprintf(KERN_DEBUG "level_entry #%zu {\n"
                        "    present:         %hhu\n"
                        "    read_write:      %hhu\n"
                        "    user_supervisor: %hhu\n"
                        "    write_through:   %hhu\n"
                        "    cache_disable:   %hhu\n"
                        "    execute_disable: %hhu\n"
                        "    accessed:        %hhu\n"
                        "    dirty:           %hhu\n"
                        "    huge_pages:      %hhu\n"
                        "    global:          %hhu\n"
                        "    phys_page_index: %#lx\n"
                        "}\n",
                        index,
                        entry->present,
                        entry->read_write,
                        entry->user_supervisor,
                        entry->write_through,
                        entry->cache_disable,
                        entry->execute_disable,
                        entry->accessed,
                        entry->dirty,
                        entry->huge_pages,
                        entry->global,
                        (uint64_t)entry->phys_page_index);
}

void vmm_print_mapping(struct cpu_context* context, virt_addr_t virt)
{
    struct paging_table* table = context->vmm_table;
    struct paging_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        if(!table) return;
        vmm_print_level_entry(table, L5_INDEX((uintptr_t)virt));

        l4 = vmm_next_table(table, L5_INDEX((uintptr_t)virt));
    }

    if(!l4) return;
    vmm_print_level_entry(l4, L4_INDEX((uintptr_t)virt));

    struct paging_table* l3 = vmm_next_table(l4, L4_INDEX((uintptr_t)virt));
    if(!l3) return;
    vmm_print_level_entry(l3, L3_INDEX((uintptr_t)virt));

    struct paging_table* l2 = vmm_next_table(l3, L3_INDEX((uintptr_t)virt));
    if(!l2) return;
    vmm_print_level_entry(l2, L2_INDEX((uintptr_t)virt));

    struct paging_table* l1 = vmm_next_table(l2, L2_INDEX((uintptr_t)virt));
    if(!l1) return;
    vmm_print_level_entry(l1, L1_INDEX((uintptr_t)virt));
}

static void vmm_destroy_table_impl(struct cpu_context* context, struct paging_table* level, size_t level_max)
{
    if(level_max == 0) return;

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(level); i++)
    {
        if(level->entries[i].present)
        {
            vmm_destroy_table_impl(context, physical_to_logical(level->entries[i].phys_page_index * PAGE_SIZE), level_max - 1);
        }
    }

    vmm_destroy_level(context, level);
}

void vmm_init_for_kernel_space(struct cpu_context* context)
{
    struct paging_table* table = vmm_create_table(context);

    context->vmm_table = (void*)table;

    vmm_map(context, 0x1000, (virt_addr_t)0x1000, 4 GiB, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    vmm_map(context, 0x1000, (virt_addr_t)(hhdm_request.response->offset + 0x1000), 4 GiB, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);

    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            vmm_map(context, entry->base, (virt_addr_t)kern_addr_request.response->virtual_base, entry->length, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
        }
    }
}

void vmm_deinit(struct cpu_context* context)
{    
    vmm_destroy_table_impl(context, (struct paging_table*)context->vmm_table, five_level_request.response != NULL ? 5 : 4);

    context->vmm_table = NULL;
}

void vmm_load(struct cpu_context* context)
{
    reg_set_cr3(logical_to_physical(context->vmm_table));
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

    if(kern_addr_request.response == NULL)
    {
        kprintf(KERN_PANIC "response to `limine_kernel_address_request` was null\n");

        return;
    }

    struct cpu_context* context = obtain_kernel_context();

    kprintf(KERN_DEBUG "Memory regions:\n");

    // The highest address is needed to compute the
    // bitmap size
    struct limine_memmap_entry* highest_entry = NULL;

    // Calculate the memory available
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        kprintf(KERN_DEBUG "    %-25s %#lx (%zu)\n", get_limine_mem_map_type_as_string(entry->type), entry->base, entry->length);

        if(!highest_entry || entry->base > highest_entry->base)
        {
            highest_entry = entry;
        }

        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            context->usable_pages += entry->length / PAGE_SIZE;
        }
        else
        {
            context->reserved_pages += entry->length / PAGE_SIZE;
        }
    }

    size_t bitmap_size = highest_entry->base + highest_entry->length; /* amount of memory needed for the bitmap */

    bitmap_size = DIV_UP(bitmap_size, PAGE_SIZE);
    bitmap_size = DIV_UP(bitmap_size, NUM_OF_BITS);

    struct limine_memmap_entry* suitable_entry = NULL;

    // Find the smallest available memory region to fit the bitmap
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type != LIMINE_MEMMAP_USABLE || entry->length < bitmap_size) continue;

        if(entry->length == bitmap_size)
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

    if(suitable_entry->length < bitmap_size)
    {
        kprintf(KERN_PANIC "Could not find an adequate memory region to place bitmap at! needed %'zu and could only find %'zu\n",
                            bitmap_size,
                            suitable_entry->length);

        return;
    }

    memory_pages.bitmap = (uint8_t*)(hhdm_request.response->offset + suitable_entry->base);
    memory_pages.size = bitmap_size;

    kprintf(KERN_DEBUG "Storing memory bitmap at %p (%'zu)\n", memory_pages.bitmap, memory_pages.size);

    // mark everything as unavailable
    memset(memory_pages.bitmap, 0xff, memory_pages.size);

    // just kidding,
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            if(pmm_mark_addr(entry->base, entry->length, false) == INVALID_PHYS)
            {
                kprintf(KERN_PANIC "error marking %#lx", entry->base);
            }
        }
    }

    // reserve bitmap
    pmm_mark_addr_and_tally(context, suitable_entry->base, bitmap_size, true);

    kprintf(KERN_INFO "Initiating virtual memory (this may take a second)\n");

    if(!(cpuid_edx(0x80000001) & (1 << 20)))
    {
        kprintf(KERN_PANIC "NX bit not supported by the CPU\n");
    }

    if(five_level_request.response != NULL)
    {
        kprintf(KERN_INFO "5-level paging enabled!\n");
    }

    vmm_init_for_kernel_space(context);
    vmm_load(context);

    kprintf(KERN_INFO "Used memory:     %'zu\n"
                      "Usable memory:   %'zu\n"
                      "Reserved memory: %'zu\n",
                      context->used_pages * PAGE_SIZE,
                      context->usable_pages * PAGE_SIZE,
                      context->reserved_pages * PAGE_SIZE);
}
