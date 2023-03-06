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

static volatile struct limine_hhdm_request hhdm_request =
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

static inline void* physical_to_logical(uintptr_t addr)
{
    return (void*)(hhdm_request.response->offset + addr);
}

static inline uintptr_t logical_to_physical(void* addr)
{
    return (uintptr_t)addr - hhdm_request.response->offset;
}

void* pmm_allocate_logical_addr(size_t size)
{
    return physical_to_logical(pmm_allocate(size));
}

void* pmm_allocate_zeroed_logical_addr(size_t size)
{
    void* addr = pmm_allocate_logical_addr(size);

    memset(addr, 0, size);

    return addr;
}

void pmm_deallocate_logical_addr(void* addr, size_t size)
{
    pmm_deallocate(logical_to_physical(addr), size);
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

struct ATTR_PACKED level_table
{
    struct level_entry entries[512];
};

#define LEVEL_TABLE_ENTRY_COUNT(TABLE_) (sizeof((TABLE_)->entries) / sizeof((TABLE_)->entries[0]))

_Static_assert(sizeof(struct level_table) == PAGE_SIZE);

static void vmm_print_level_entry(const struct level_table* table, size_t index)
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

static inline struct level_entry vmm_create_level_entry(uintptr_t phys_addr, uint32_t flags)
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

struct level_table* vmm_create_table(void)
{
    return pmm_allocate_zeroed_logical_addr(PAGE_SIZE);
}

static inline void vmm_destroy_level(struct level_table* level)
{
    pmm_deallocate_logical_addr(level, PAGE_SIZE);
}

static struct level_table* vmm_next_table(struct level_table* table, size_t index)
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

static struct level_table* vmm_next_table_or_alloc(struct level_table* table, size_t index, uint32_t flags)
{
    struct level_table* next_table = 0;

    if(!table || index >= LEVEL_TABLE_ENTRY_COUNT(table))
    {
        kprintf(KERN_PANIC "vmm_next_table_or_alloc: invalid index or table\n");
    }

    if(table->entries[index].present)
    {
        next_table = physical_to_logical(table->entries[index].phys_page_index * PAGE_SIZE);
    }
    else
    {
        next_table = vmm_create_table();
        table->entries[index] = vmm_create_level_entry(logical_to_physical(next_table), flags);
    }

    return next_table;
}

void vmm_map_page(struct level_table* table, uintptr_t phys, void* virt, uint32_t flags)
{
    struct level_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        l4 = vmm_next_table_or_alloc(table, L5_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }

    struct level_table* l3 = vmm_next_table_or_alloc(l4, L4_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    struct level_table* l2 = vmm_next_table_or_alloc(l3, L3_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    struct level_table* l1 = vmm_next_table_or_alloc(l2, L2_INDEX((uintptr_t)virt), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);

    struct level_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        *entry = vmm_create_level_entry(phys, flags);
    }
    else
    {
        kprintf(KERN_PANIC "vmm_map: %p is already mapped\n", virt);
    }

    if(!(flags & MAP_PAGE_NO_FLUSH) && reg_get_cr3() == logical_to_physical(table))
    {
        invalidate_page(virt);
    }
}

void vmm_map(struct level_table* table, uintptr_t phys, void* virt, size_t size, uint32_t flags)
{
    for(size_t off = 0; off < size; off += PAGE_SIZE)
    {
        vmm_map_page(table, phys + off, (void*)(((uintptr_t)virt) + off), flags);
    }
}

static void vmm_free_table_if_needed(struct level_table* parent, size_t index)
{
    struct level_table* table = physical_to_logical(parent->entries[index].phys_page_index * PAGE_SIZE);

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(table); i++)
    {
        if(table->entries[i].present)
        {
            return;
        }
    }

    vmm_destroy_level(table);

    parent->entries[index] = vmm_create_null_level_entry();
}

void vmm_unmap_page(struct level_table* table, void* virt)
{
    struct level_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        l4 = vmm_next_table(table, L5_INDEX((uintptr_t)virt));
    }

    struct level_table* l3 = vmm_next_table(l4, L4_INDEX((uintptr_t)virt));
    struct level_table* l2 = vmm_next_table(l3, L3_INDEX((uintptr_t)virt));
    struct level_table* l1 = vmm_next_table(l2, L2_INDEX((uintptr_t)virt));

    struct level_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        kprintf(KERN_PANIC "vmm_unmap: %p not mapped", virt);
    }

    *entry = vmm_create_null_level_entry();

    // we should free the levels if needed
    // but not the top level
    vmm_free_table_if_needed(l2, L2_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(l3, L3_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(l4, L4_INDEX((uintptr_t)virt));

    if(five_level_request.response != NULL)
    {
        vmm_free_table_if_needed(table, L5_INDEX((uintptr_t)virt));
    }
}

void vmm_unmap(struct level_table* table, void* virt, size_t size)
{
    for(size_t off = 0; off < size; off += PAGE_SIZE)
    {
        vmm_unmap_page(table, (void*)(((uintptr_t)virt) + off));
    }
}

static void vmm_print_mapping(struct level_table* table, void* virt)
{
    struct level_table* l4 = table;

    if(five_level_request.response != NULL)
    {
        vmm_print_level_entry(table, L5_INDEX((uintptr_t)virt));

        l4 = vmm_next_table(table, L5_INDEX((uintptr_t)virt));
    }

    vmm_print_level_entry(l4, L4_INDEX((uintptr_t)virt));

    struct level_table* l3 = vmm_next_table(l4, L4_INDEX((uintptr_t)virt));
    vmm_print_level_entry(l3, L3_INDEX((uintptr_t)virt));

    struct level_table* l2 = vmm_next_table(l3, L3_INDEX((uintptr_t)virt));
    vmm_print_level_entry(l2, L2_INDEX((uintptr_t)virt));

    struct level_table* l1 = vmm_next_table(l2, L2_INDEX((uintptr_t)virt));
    vmm_print_level_entry(l1, L1_INDEX((uintptr_t)virt));
}

static void vmm_print_all_mappings_impl(struct level_table* level, uintptr_t addr, size_t level_max)
{
    if(level_max == 0) return;

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(level); i++)
    {
        if(level->entries[i].present)
        {
            uintptr_t a = addr;

            a <<= 12;
            a |= i;

            if(level_max == 1)
            {
                struct level_entry* entry = &level->entries[i];

                a <<= 12;
                a |= entry->phys_page_index;

                kprintf("%#lx-%#lx-%#lx-%#lx => %#lx\n", L1_INDEX(a), L2_INDEX(a), L3_INDEX(a), L4_INDEX(a), (uintptr_t)(entry->phys_page_index * PAGE_SIZE));
            }

            vmm_print_all_mappings_impl(physical_to_logical(level->entries[i].phys_page_index * PAGE_SIZE), a, level_max - 1);
        }
    }
}

static void vmm_print_all_mappings(struct level_table* level)
{
    vmm_print_all_mappings_impl(level, 0, five_level_request.response != NULL ? 5 : 4);
}

static void vmm_destroy_table_impl(struct level_table* level, size_t level_max)
{
    if(level_max == 0) return;

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(level); i++)
    {
        if(level->entries[i].present)
        {
            vmm_destroy_table_impl(physical_to_logical(level->entries[i].phys_page_index * PAGE_SIZE), level_max - 1);
        }
    }

    vmm_destroy_level(level);
}

void vmm_destroy_table(struct level_table* table)
{    
    vmm_destroy_table_impl(table, five_level_request.response != NULL ? 5 : 4);
}

void vmm_load_table(struct level_table* table)
{
    reg_set_cr3(logical_to_physical(table));
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

    if(five_level_request.response != NULL)
    {
        kprintf(KERN_INFO "5-level paging enabled!\n");
    }
    else if(cpuid_ecx_with_subleaf(7, 0) & (1 << 16))
    {
        // I clearly don't trust limine
        kprintf(KERN_WARN "5-level paging is supported by the system but limine won't enable it!\n");
    }

    if(kern_addr_request.response)
    {
        kprintf("kern_phys: %#lx\nkern_virt: %#lx\n", kern_addr_request.response->physical_base, kern_addr_request.response->virtual_base);
    }

    kprintf(KERN_INFO "Initiating virtual memory (this may take a little while)\n");

    struct level_table* root_table = vmm_create_table();

    vmm_map(root_table, 0x1000, (void*)0x1000, 4 GiB, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    vmm_map(root_table, 0x1000, (void*)(hhdm_request.response->offset + 0x1000), 4 GiB, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);

    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            vmm_map(root_table, entry->base, (void*)kern_addr_request.response->virtual_base, entry->length, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
        }
    }

    vmm_load_table(root_table);

    kprintf(KERN_INFO "Used memory:     %'zu\n"
                      "Usable memory:   %'zu\n"
                      "Reserved memory: %'zu\n",
                      memory_pages.used_pages * PAGE_SIZE,
                      memory_pages.usable_pages * PAGE_SIZE,
                      memory_pages.reserved_pages * PAGE_SIZE);
}
