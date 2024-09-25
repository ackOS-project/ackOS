#include <string.h>

#include <limine/limine.h>
#include <liback/util.h>

#include "kernel/arch/x86_64/mem.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/cpuid.h"
#include "kernel/mm/phys.h"
#include "kernel/mm/mem.h"
#include "kernel/lib/log.h"
#include "kernel/lib/pointless.h"

#define NUM_OF_BITS 8

// enable five level paging if available
static volatile struct limine_paging_mode_request five_level_request =
{
    .id = LIMINE_PAGING_MODE_REQUEST,
    .mode = LIMINE_PAGING_MODE_X86_64_5LVL,
    .flags = 0,
    .revision = 0
};

static bool five_level_paging_enabled = false;
static size_t phys_address_bits_num = 0;
static bool large_1gib_page_supported = false;
static virt_addr_t direct_map_offset = NULL;
static virt_addr_t kern_virt_addr = NULL;
static phys_addr_t kern_phys_addr = INVALID_PHYS_ADDR;

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

struct vmm_context kernel_vmm_context;

static inline size_t size_to_num_of_pages(size_t size)
{
    return DIV_UP(size, PAGE_SIZE);
}

inline virt_addr_t physical_to_io(phys_addr_t addr)
{
    if(addr >= 4 GiB) return NULL;

    return (virt_addr_t)(direct_map_offset + addr);
}

inline phys_addr_t io_to_physical(virt_addr_t addr)
{
    if((uintptr_t)addr < (uintptr_t)direct_map_offset || (uintptr_t)addr >= ((uintptr_t)direct_map_offset + 4 GiB)) return INVALID_PHYS_ADDR;

    return (phys_addr_t)(addr - direct_map_offset);
}

virt_addr_t pmm_allocate_logical_addr(size_t size)
{
    phys_addr_t addr = pmm_allocate(size);

    if(addr == INVALID_PHYS_ADDR) return NULL;

    return physical_to_io(addr);
}

virt_addr_t pmm_allocate_zeroed_logical_addr(size_t size)
{
    virt_addr_t addr = pmm_allocate_logical_addr(size);

    if(addr != NULL)
    {
        memset(addr, 0, size);
    }

    return addr;
}

void pmm_deallocate_logical_addr(virt_addr_t addr, size_t size)
{
    pmm_deallocate(io_to_physical(addr), size);
}

// VMM
/*
An overview of the x86_64 paging structure:
    Every virtual address follows this structure:
        L5 (9 bits) | L4 (9 bits) | L3 (9 bits) | L2 (9 bits) | L1 (9 bits) | offset (12 bits)
    
    please note: 5 level paging is only supported by some systems and is not enabled by default
    
    Which corresponds to this hierarchy of a page table trie:
        level 5 (PML - covers 128PiB of address space)
            level 4 (PML - covers 256TiB of address space)
                level 3 (PDP - covers 512GiB of address space. Entries can optionally behave as page mapping allowing for a 1GiB mapping.)
                  * level 2 (PD - covers 1GiB of address space. Entries can optionally behave as page mapping allowing for a 2MiB mapping.)
                      * level 1 (PT - covers 2MiB of address space.)
                            512 entries (4KiB page mappings)

    As you see, levels 2 and 1 are special as they can be used to map larger page granularities. This is important for performance.

References:
https://cs.brown.edu/courses/csci0300/2021/notes/l13.html
https://en.wikipedia.org/wiki/Intel_5-level_paging#/media/File:Page_Tables_(5_levels).svg
https://wiki.osdev.org/File:64-bit_page_tables1.png
https://wiki.osdev.org/File:64-bit_page_tables2.png
Intel Manual "Figure 4-7. Formats of CR3 and Paging-Structure entries with PAE paging"
*/

// also there is lower half and higher half memory
// typically the lower half address space is reserved for userspace applications
// while the higher half memory is reserved for the kernel
// On 4 level paging systems virtual addresses are respresented using 48 bit signed integers
// ranging from -0x800000000000 (-2^47) to 0x7fffffffffff (2^47 - 1). These addresses are typically
// not printed in signed form rather they are shown in 64-bit two's complement sign extended form, where the negative address
// are prefixed with 0xffff-. Under this system, the higher half begins at 0xffff_8000_0000_0000 and ends at 0xffff_ffff_ffff_ffff,
// covering half of address space (128 TiB out of 256TiB). Likewise the lower half starts at 0x0000000000000000 and goes up to 0x7fffffffffff.
// Two's complement can be a bit confusing so I recommend watching Ben Eater's video: https://www.youtube.com/watch?v=4qH4unVtJkE
static void print_signed_address(const char* label, intptr_t addr)
{
    kprintf(KERN_DEBUG "%s = %#lx = %c%#lx\n", label, (uintptr_t)addr, addr < 0 ? '-' : '+', addr < 0 ? -addr : addr);
}

#define L5_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 48)) >> 48)
#define L4_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 39)) >> 39)
#define L3_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 30)) >> 30)
#define L2_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 21)) >> 21)
#define L1_INDEX(ADDR_) ((ADDR_ & ((uint64_t)0x1ff << 12)) >> 12)

struct ATTR_PACKED page_entry
{
    uint8_t present : 1,
            read_write : 1,
            user_supervisor : 1,
            write_through : 1,
            cache_disable : 1,
            accessed : 1,
            dirty : 1,
            large_page_or_pat : 1;
    uint8_t the_rest[6];
    uint8_t : 3,
            protection_key : 4,
            execute_disable : 1;
};

_Static_assert(sizeof(struct page_entry) == sizeof(uint64_t));

static inline phys_addr_t vmm_page_entry_get_phys(const struct page_entry* entry, const int level)
{
    uint64_t raw = *((uint64_t*)entry);

    if(level == 2 && entry->large_page_or_pat)
    {
        return (phys_addr_t)(((raw & (((1UL << (phys_address_bits_num - 30)) - 1) << 30)) >> 30) * 1 GiB);
    }
    else if(level == 1 && entry->large_page_or_pat)
    {
        return (phys_addr_t)(((raw & (((1UL << (phys_address_bits_num - 21)) - 1) << 21)) >> 21) * 2 MiB);
    }

    return (phys_addr_t)(((raw & (((1UL << (phys_address_bits_num - 12)) - 1) << 12)) >> 12) * PAGE_SIZE);
}

static inline bool vmm_page_entry_set_phys(struct page_entry* entry, const int level, phys_addr_t addr)
{
    union {
        uint64_t raw;
        struct page_entry entry;
    } raw;

    raw.entry = *entry;

    if(level == 2 && entry->large_page_or_pat)
    {
        uint64_t phys_mask = (1UL << (phys_address_bits_num - 30)) - 1;
        uint64_t phys_index = addr / (1 GiB);

        if(phys_index > phys_mask)
        {
            return false;
        }

        // clear address bits
        raw.raw &= ~(phys_mask << 30);
        // set address bits
        raw.raw |= phys_index << 30;

        *entry = raw.entry;

        return true;
    }
    if(level == 1 && entry->large_page_or_pat)
    {
        uint64_t phys_mask = (1UL << (phys_address_bits_num - 21)) - 1;
        uint64_t phys_index = addr / (2 MiB);

        if(phys_index > phys_mask)
        {
            return false;
        }

        raw.raw &= ~(phys_mask << 21);
        raw.raw |= phys_index << 21;

        *entry = raw.entry;

        return true;
    }

    uint64_t phys_mask = (1UL << (phys_address_bits_num - 12)) - 1;
    uint64_t phys_index = addr / PAGE_SIZE;

    if(phys_index > phys_mask)
    {
        return false;
    }

    raw.raw &= ~(phys_mask << 12);
    raw.raw |= phys_index << 12;

    *entry = raw.entry;

    return true;
}

static inline bool vmm_page_entry_is_leaf(const struct page_entry* entry, const int level)
{
    return level == 0 || ((level == 2 || level == 1) && entry->large_page_or_pat);
}

struct ATTR_PACKED paging_table
{
    struct page_entry entries[512];
};

#define LEVEL_TABLE_ENTRY_COUNT(TABLE_) (sizeof((TABLE_)->entries) / sizeof((TABLE_)->entries[0]))

_Static_assert(sizeof(struct paging_table) == PAGE_SIZE);

static inline struct page_entry vmm_create_null_page_entry(void)
{
    struct page_entry entry = {0};

    return entry;
}

static inline struct page_entry vmm_create_page_entry(const int level, phys_addr_t phys_addr, uint32_t flags)
{
    struct page_entry entry = {0};

    entry.present = true;
    entry.read_write = (flags & MAP_PAGE_WRITE) != 0;
    entry.user_supervisor = (flags & MAP_PAGE_USER) != 0;
    entry.write_through = false;
    entry.cache_disable = false;
    entry.accessed = false;
    entry.dirty = false;
    entry.large_page_or_pat = false;
    entry.execute_disable = (flags & MAP_PAGE_EXECUTE) == 0;

    if(!vmm_page_entry_set_phys(&entry, level, phys_addr))
    {
        return vmm_create_null_page_entry();
    }

    return entry;
}

static inline struct page_entry vmm_create_large_page_entry(const int level, phys_addr_t phys_addr, uint32_t flags)
{
    struct page_entry entry = {0};

    if(level == 2 || level == 1) entry.large_page_or_pat = true;

    entry.present = true;
    entry.read_write = (flags & MAP_PAGE_WRITE) != 0;
    entry.user_supervisor = (flags & MAP_PAGE_USER) != 0;
    entry.write_through = false;
    entry.cache_disable = false;
    entry.accessed = false;
    entry.dirty = false;
    entry.execute_disable = (flags & MAP_PAGE_EXECUTE) == 0;

    if(!vmm_page_entry_set_phys(&entry, level, phys_addr))
    {
        return vmm_create_null_page_entry();
    }

    return entry;
}

static struct paging_table* vmm_create_table(void)
{
    return (struct paging_table*)pmm_allocate_zeroed_logical_addr(PAGE_SIZE);
}

static inline void vmm_destroy_level(struct paging_table* level)
{
    pmm_deallocate_logical_addr(level, PAGE_SIZE);
}

void vmm_init(struct vmm_context* context)
{
    context->vmm_table = (void*)vmm_create_table();
}

bool vmm_map_page(struct vmm_context* context, phys_addr_t phys, virt_addr_t virt, uint32_t flags)
{
    if(phys % PAGE_SIZE != 0 || (uintptr_t)virt % PAGE_SIZE != 0)
    {
        return false;
    }

    struct paging_table* table = context->vmm_table;

    if(!table) return false;
    
    struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            l4 = vmm_create_table();
            *l4_entry = vmm_create_page_entry(4, io_to_physical(l4), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
        }
        else
        {
            l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
        }
    }

    // L3
    struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];
    struct paging_table* l3 = NULL;

    if(!l3_entry->present)
    {
        l3 = vmm_create_table();
        *l3_entry = vmm_create_page_entry(3, io_to_physical(l3), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else
    {
        l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));
    }

    // L2
    struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];
    struct paging_table* l2 = NULL;

    if(!l2_entry->present)
    {
        l2 = vmm_create_table();
        *l2_entry = vmm_create_page_entry(2, io_to_physical(l2), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else if(l2_entry->large_page_or_pat)
    {
        return false;
    }
    else
    {
        l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));
    }

    // L1
    struct page_entry* l1_entry = &l2->entries[L2_INDEX((uintptr_t)virt)];
    struct paging_table* l1 = NULL;

    if(!l1_entry->present)
    {
        l1 = vmm_create_table();
        *l1_entry = vmm_create_page_entry(1, io_to_physical(l1), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else if(l1_entry->large_page_or_pat)
    {
        return false;
    }
    else
    {
        l1 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l1_entry, 1));
    }

    struct page_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(entry->present)
    {
        // already mapped
        return false;
    }

    *entry = vmm_create_page_entry(0, phys, flags);

    if(!entry->present)
    {
        // if page entry is not present it probably wasn't created
        // properly

        return false;
    }

    if(!(flags & MAP_NO_FLUSH) && reg_get_cr3() == io_to_physical(table))
    {
        invalidate_page(virt);
    }

    return true;
}

bool vmm_map_1gib_page(struct vmm_context* context, phys_addr_t phys, virt_addr_t virt, uint32_t flags)
{
    if(!large_1gib_page_supported || phys % (1 GiB) != 0 || (uintptr_t)virt % (1 GiB) != 0)
    {
        return false;
    }

    struct paging_table* table = context->vmm_table;

    if(!table) return false;
    
    struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            l4 = vmm_create_table();
            *l4_entry = vmm_create_page_entry(4, io_to_physical(l4), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
        }
        else
        {
            l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
        }
    }

    // L3
    struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];
    struct paging_table* l3 = NULL;

    if(!l3_entry->present)
    {
        l3 = vmm_create_table();
        *l3_entry = vmm_create_page_entry(3, io_to_physical(l3), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else
    {
        l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));
    }

    // L2
    struct page_entry* entry = &l3->entries[L3_INDEX((uintptr_t)virt)];

    if(entry->present)
    {
        // already mapped
        return false;
    }

    *entry = vmm_create_large_page_entry(2, phys, flags);

    if(!entry->present)
    {
        // if page entry is not present it probably wasn't created
        // properly

        return false;
    }

    if(!(flags & MAP_NO_FLUSH) && reg_get_cr3() == io_to_physical(table))
    {
        invalidate_page(virt);
    }

    return true;
}

bool vmm_map_2mib_page(struct vmm_context* context, phys_addr_t phys, virt_addr_t virt, uint32_t flags)
{
    if(phys % (2 MiB) != 0 || (uintptr_t)virt % (2 MiB) != 0)
    {
        return false;
    }

    struct paging_table* table = context->vmm_table;

    if(!table) return false;
    
    struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            l4 = vmm_create_table();
            *l4_entry = vmm_create_page_entry(4, io_to_physical(l4), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
        }
        else
        {
            l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
        }
    }

    // L3
    struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];
    struct paging_table* l3 = NULL;

    if(!l3_entry->present)
    {
        l3 = vmm_create_table();
        *l3_entry = vmm_create_page_entry(3, io_to_physical(l3), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else
    {
        l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));
    }

    // L2
    struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];
    struct paging_table* l2 = NULL;

    if(!l2_entry->present)
    {
        l2 = vmm_create_table();
        *l2_entry = vmm_create_page_entry(2, io_to_physical(l2), MAP_PAGE_USER | MAP_PAGE_WRITE | MAP_PAGE_EXECUTE);
    }
    else if(l2_entry->large_page_or_pat)
    {
        return false;
    }
    else
    {
        l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));
    }

    // L1
    struct page_entry* entry = &l2->entries[L2_INDEX((uintptr_t)virt)];

    if(entry->present)
    {
        // already mapped
        return false;
    }

    *entry = vmm_create_large_page_entry(1, phys, flags);

    if(!entry->present)
    {
        // if page entry is not present it probably wasn't created
        // properly

        return false;
    }

    if(!(flags & MAP_NO_FLUSH) && reg_get_cr3() == io_to_physical(table))
    {
        invalidate_page(virt);
    }

    return true;
}

bool vmm_map(struct vmm_context* context, phys_addr_t phys, virt_addr_t virt, size_t size, uint32_t flags)
{
    for(size_t off = 0; off < size;)
    {
        const size_t remaining_bytes = size - off;
        const phys_addr_t phys_addr = phys + off;
        const virt_addr_t virt_addr = (virt_addr_t)(((uintptr_t)virt) + off);

        if ((flags & MAP_LARGE_PAGES) && large_1gib_page_supported && remaining_bytes >= 1 GiB && vmm_map_1gib_page(context, phys_addr, virt_addr, flags))
        {
            off += 1 GiB;
        }
        else if ((flags & MAP_LARGE_PAGES) && remaining_bytes >= 2 MiB && vmm_map_2mib_page(context, phys_addr, virt_addr, flags))
        {
            off += 2 MiB;
        }
        else if(vmm_map_page(context, phys_addr, virt_addr, flags))
        {
            off += PAGE_SIZE;
        }
        else
        {
            // undo previous mappings
            vmm_unmap(context, virt, off);

            return false;
        }
    }

    return true;
}

static void vmm_free_table_if_needed(struct vmm_context* context, const int level, struct paging_table* parent, size_t index)
{
    struct paging_table* table = physical_to_io(vmm_page_entry_get_phys(&parent->entries[index], level));

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(table); i++)
    {
        if(table->entries[i].present)
        {
            return;
        }
    }

    vmm_destroy_level(table);

    parent->entries[index] = vmm_create_null_page_entry();
}

size_t vmm_unmap_page(struct vmm_context* context, virt_addr_t virt)
{
    if((uintptr_t)virt % PAGE_SIZE != 0)
    {
        return 0;
    }

    struct paging_table* table = context->vmm_table;

    if(!table) return 0;
    
    struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            return 0;
        }

        l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
    }

    // L3
    struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];

    if(!l3_entry->present)
    {
        return 0;
    }

    struct paging_table* l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));

    // L2
    struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];

    if(!l2_entry->present)
    {
        return 0;
    }

    if (l2_entry->large_page_or_pat)
    {
        *l2_entry = vmm_create_null_page_entry();

        vmm_free_table_if_needed(context, 3, l4, L4_INDEX((uintptr_t)virt));

        if(five_level_paging_enabled)
        {
            vmm_free_table_if_needed(context, 4, table, L5_INDEX((uintptr_t)virt));
        }

        return 1 GiB;
    }

    struct paging_table* l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));

    // L1
    struct page_entry* l1_entry = &l2->entries[L2_INDEX((uintptr_t)virt)];

    if(!l1_entry->present)
    {
        return 0;
    }

    if (l1_entry->large_page_or_pat)
    {
        *l1_entry = vmm_create_null_page_entry();

        vmm_free_table_if_needed(context, 2, l3, L3_INDEX((uintptr_t)virt));
        vmm_free_table_if_needed(context, 3, l4, L4_INDEX((uintptr_t)virt));

        if(five_level_paging_enabled)
        {
            vmm_free_table_if_needed(context, 4, table, L5_INDEX((uintptr_t)virt));
        }

        return 2 MiB;
    }

    struct paging_table* l1 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l1_entry, 1));

    struct page_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        // not mapped
        return 0;
    }

    *entry = vmm_create_null_page_entry();

    // we should free the levels if needed
    // but not the top level
    vmm_free_table_if_needed(context, 1, l2, L2_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(context, 2, l3, L3_INDEX((uintptr_t)virt));
    vmm_free_table_if_needed(context, 3, l4, L4_INDEX((uintptr_t)virt));

    if(five_level_paging_enabled)
    {
        vmm_free_table_if_needed(context, 4, table, L5_INDEX((uintptr_t)virt));
    }

    return PAGE_SIZE;
}

bool vmm_unmap(struct vmm_context* context, virt_addr_t virt, size_t size)
{
    for(size_t off = 0; off < size;)
    {
        size_t amount_unmapped = vmm_unmap_page(context, (virt_addr_t)(((uintptr_t)virt) + off));

        if (amount_unmapped > 0)
        {
            off += amount_unmapped;
        }
        else
        {
            return false;
        }
    }

    return true;
}

phys_addr_t vmm_virt_to_phys(const struct vmm_context* context, virt_addr_t virt)
{
    if((uintptr_t)virt % PAGE_SIZE != 0)
    {
        return INVALID_PHYS_ADDR;
    }

    const struct paging_table* table = context->vmm_table;

    if(!table) return INVALID_PHYS_ADDR;
    
    const struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        const struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            return INVALID_PHYS_ADDR;
        }

        l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
    }

    // L3
    const struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];

    if(!l3_entry->present)
    {
        return INVALID_PHYS_ADDR;
    }

    const struct paging_table* l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));

    // L2
    const struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];

    if(!l2_entry->present)
    {
        return INVALID_PHYS_ADDR;
    }

    if(l2_entry->large_page_or_pat)
    {
        return vmm_page_entry_get_phys(l2_entry, 2);
    }

    const struct paging_table* l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));

    // L1
    const struct page_entry* l1_entry = &l2->entries[L2_INDEX((uintptr_t)virt)];

    if(!l1_entry->present)
    {
        return INVALID_PHYS_ADDR;
    }

    if(l1_entry->large_page_or_pat)
    {
        return vmm_page_entry_get_phys(l1_entry, 1);
    }

    struct paging_table* l1 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l1_entry, 1));
    const struct page_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        return INVALID_PHYS_ADDR;
    }

    return vmm_page_entry_get_phys(entry, 0);
}

const struct page_entry* vmm_get_page_entry_leaf(const struct vmm_context* context, virt_addr_t virt, int* level)
{
    const struct paging_table* table = context->vmm_table;
    const struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        const struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            return NULL;
        }

        l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
    }

    // L3
    const struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];

    if(!l3_entry->present)
    {
        return NULL;
    }

    const struct paging_table* l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));

    // L2
    const struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];

    if(!l2_entry->present)
    {
        return NULL;
    }

    if(l2_entry->large_page_or_pat)
    {
        *level = 2;

        return l2_entry;
    }

    const struct paging_table* l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));

    // L1
    const struct page_entry* l1_entry = &l2->entries[L2_INDEX((uintptr_t)virt)];

    if(!l1_entry->present)
    {
        return NULL;
    }

    if(l1_entry->large_page_or_pat)
    {
        *level = 1;

        return l1_entry;
    }

    struct paging_table* l1 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l1_entry, 1));
    const struct page_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        return NULL;
    }

    *level = 0;

    return entry;
}

size_t vmm_get_address_space(void)
{
    int level = five_level_paging_enabled ? 5 : 4;
    size_t size = 1;

    // formula: (512 ^ level) * PAGE_SIZE

    for(int i = 1; i <= level; i++)
    {
        size *= 512;
    }

    size *= PAGE_SIZE;

    return size;
}

static void vmm_print_page_entry(const int level, const struct paging_table* table, size_t index)
{
    const struct page_entry* entry = &table->entries[index];

    kprintf(KERN_DEBUG "page_entry @L%i[%zu] {\n"
                        "    present:         %hhu\n"
                        "    read_write:      %hhu\n"
                        "    user_supervisor: %hhu\n"
                        "    write_through:   %hhu\n"
                        "    cache_disable:   %hhu\n"
                        "    execute_disable: %hhu\n"
                        "    accessed:        %hhu\n"
                        "    %s_addr:       %#lx\n"
                        "} = %#lx\n",
                        level,
                        index,
                        entry->present,
                        entry->read_write,
                        entry->user_supervisor,
                        entry->write_through,
                        entry->cache_disable,
                        entry->execute_disable,
                        entry->accessed,
                        vmm_page_entry_is_leaf(entry, level - 1) ? "phys" : "next",
                        vmm_page_entry_get_phys(entry, level - 1),
                        *(uint64_t*)entry);
}

void vmm_print_mapping(const struct vmm_context* context, virt_addr_t virt)
{
    if((uintptr_t)virt % PAGE_SIZE != 0)
    {
        kprintf(KERN_DEBUG "vmm_print_mapping: virtual address %p not page alligned\n", virt);

        return;
    }

    struct paging_table* table = context->vmm_table;

    if(!table) return;
    
    struct paging_table* l4 = table;

    if(five_level_paging_enabled)
    {
        struct page_entry* l4_entry = &table->entries[L5_INDEX((uintptr_t)virt)];

        if(!l4_entry->present)
        {
            return;
        }

        vmm_print_page_entry(5, table, L5_INDEX((uintptr_t)virt));

        l4 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l4_entry, 4));
    }

    // L3
    struct page_entry* l3_entry = &l4->entries[L4_INDEX((uintptr_t)virt)];

    if(!l3_entry->present)
    {
        return;
    }

    vmm_print_page_entry(4, l4, L4_INDEX((uintptr_t)virt));

    struct paging_table* l3 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l3_entry, 3));

    // L2
    struct page_entry* l2_entry = &l3->entries[L3_INDEX((uintptr_t)virt)];

    if(!l2_entry->present)
    {
        return;
    }

    vmm_print_page_entry(3, l3, L3_INDEX((uintptr_t)virt));

    if(l2_entry->large_page_or_pat)
    {
        return;
    }

    struct paging_table* l2 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l2_entry, 2));

    // L1
    struct page_entry* l1_entry = &l2->entries[L2_INDEX((uintptr_t)virt)];

    if(!l1_entry->present)
    {
        return;
    }

    vmm_print_page_entry(2, l2, L2_INDEX((uintptr_t)virt));

    if(l1_entry->large_page_or_pat)
    {
        return;
    }

    struct paging_table* l1 = (struct paging_table*)physical_to_io(vmm_page_entry_get_phys(l1_entry, 1));

    struct page_entry* entry = &l1->entries[L1_INDEX((uintptr_t)virt)];

    if(!entry->present)
    {
        return;
    }

    vmm_print_page_entry(1, l1, L1_INDEX((uintptr_t)virt));
}

static void vmm_destroy_table_impl(struct vmm_context* context, struct paging_table* level, size_t level_max)
{
    if(level_max == 0) return;

    for(size_t i = 0; i < LEVEL_TABLE_ENTRY_COUNT(level); i++)
    {
        if(vmm_page_entry_is_leaf(&level->entries[i], level_max - 1)) continue;

        if(level->entries[i].present)
        {
            vmm_destroy_table_impl(context, physical_to_io(vmm_page_entry_get_phys(&level->entries[i], level_max - 1)), level_max - 1);
        }
    }

    vmm_destroy_level(level);
}

void vmm_init_for_kernel_space(struct vmm_context* context)
{
    struct paging_table* table = vmm_create_table();

    context->vmm_table = (void*)table;

    if(!vmm_map(context, 0, (virt_addr_t)direct_map_offset, 4 GiB, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE | MAP_LARGE_PAGES))
    {
        kprintf(KERN_PANIC "Failed to map first 4GiB\n");
    }

    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if(entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            if(!vmm_map(context, entry->base, kern_virt_addr, entry->length, MAP_PAGE_WRITE | MAP_PAGE_EXECUTE))
            {
                kprintf(KERN_PANIC "Failed to map kernel (addr=%#lx)\n", entry->base);
            }

            break;
        }
    }
}

void vmm_deinit(struct vmm_context* context)
{    
    vmm_destroy_table_impl(context, (struct paging_table*)context->vmm_table, five_level_paging_enabled ? 5 : 4);

    context->vmm_table = NULL;
}

void vmm_load(struct vmm_context* context)
{
    reg_set_cr3(io_to_physical(context->vmm_table));
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

#include "kernel/arch/x86_64/com.h"

static volatile struct limine_smp_request smp_request =
{
    .id = LIMINE_SMP_REQUEST,
    .flags = 0,
    .revision = 0
};

#define FULL_MEMMAP_OFFSET ((virt_addr_t)((uintptr_t)direct_map_offset + 4 GiB))

virt_addr_t vmm_allocate(struct vmm_context* context, phys_addr_t addr, size_t size, uint32_t flags)
{
    if (addr % PAGE_SIZE != 0)
    {
        return NULL;
    }

    // reuse the <4GiB mapping if possible
    // The only flag that this mapping does not
    // have is MAP_PAGE_USER
    if (addr < 4 GiB && (addr + DIV_UP(size, PAGE_SIZE) * PAGE_SIZE) < 4 GiB && !(flags & MAP_PAGE_USER))
    {
        return (virt_addr_t)((uintptr_t)direct_map_offset + (uintptr_t)addr);
    }

    virt_addr_t virt_addr = FULL_MEMMAP_OFFSET + addr;

    if (!vmm_map(context, addr, virt_addr, size, flags))
    {
        return NULL;
    }

    return virt_addr;
}

phys_addr_t vmm_deallocate(struct vmm_context* context, virt_addr_t addr, size_t size)
{
    if((uintptr_t)addr % PAGE_SIZE != 0)
    {
        return INVALID_PHYS_ADDR;
    }

    phys_addr_t phys_addr = INVALID_PHYS_ADDR;

    if (addr >= FULL_MEMMAP_OFFSET)
    {
        phys_addr = addr - FULL_MEMMAP_OFFSET;

        if (!vmm_unmap(context, addr, size)) return INVALID_PHYS_ADDR;
    }
    else if (addr >= direct_map_offset)
    {
        phys_addr = addr - direct_map_offset;
    }

    return phys_addr;
}

extern void pmm_init(uint8_t* bitmap_memory, size_t size, size_t used_pages, size_t usable_pages, size_t reserved_pages);

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

    direct_map_offset = (virt_addr_t)hhdm_request.response->offset;
    kern_virt_addr = (virt_addr_t)kern_addr_request.response->virtual_base;
    kern_phys_addr = (phys_addr_t)kern_addr_request.response->physical_base;

    kprintf(KERN_DEBUG "Memory regions:\n");

    // The highest address is needed to compute the
    // bitmap size
    struct limine_memmap_entry* highest_entry = NULL;
    size_t used_pages = 0, usable_pages = 0, reserved_pages = 0;

    // Calculate the memory available
    for(size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        kprintf(KERN_DEBUG "    %-25s %#lx (%'zu)\n", get_limine_mem_map_type_as_string(entry->type), entry->base, entry->length);

        if(!highest_entry || entry->base > highest_entry->base)
        {
            highest_entry = entry;
        }

        size_t n_pages = entry->length / PAGE_SIZE;

        if(entry->type == LIMINE_MEMMAP_USABLE)
        {
            usable_pages += n_pages;
        }
        else if(entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
                entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
                entry->type == LIMINE_MEMMAP_KERNEL_AND_MODULES)
        {
            used_pages += n_pages;
            usable_pages += n_pages;
        }
        else
        {
            reserved_pages += n_pages;
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

    pmm_init((uint8_t*)(direct_map_offset + suitable_entry->base), bitmap_size, used_pages, usable_pages, reserved_pages);

    // mark the pages that are usable as unreserved
    for (size_t i = 0; i < memmap_request.response->entry_count; i++)
    {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            if (pmm_mark_addr(entry->base, entry->length, false) == INVALID_PHYS_ADDR)
            {
                kprintf(KERN_PANIC "error marking %#lx", entry->base);
            }
        }
    }

    // reserve bitmap
    pmm_mark_addr_and_tally(suitable_entry->base, bitmap_size, true);

    if(!(cpuid_edx(0x80000001) & (1 << 20)))
    {
        kprintf(KERN_PANIC "NX bit not supported by the CPU\n");
    }

    five_level_paging_enabled = five_level_request.response != NULL && five_level_request.response->mode == LIMINE_PAGING_MODE_X86_64_5LVL;
    phys_address_bits_num = (size_t)(cpuid_eax(0x80000008) & 0x7f);
    large_1gib_page_supported = ((cpuid_edx(0x80000001) & (1 << 26)) >> 26);

    if (!large_1gib_page_supported)
    {
        kprintf(KERN_WARN "1 GiB large pages are not supported by the CPU!\n");
    }

    kprintf(KERN_INFO "Total virtual address space: %'zu\n", vmm_get_address_space());

    vmm_init_for_kernel_space(&kernel_vmm_context);
    vmm_load(&kernel_vmm_context);

    struct memory_info meminfo = get_memory_info();

    kprintf(KERN_INFO "Usable memory:   %'zu\n"
                      "Used memory:     %'zu (%zu%% used)\n"
                      "Reserved memory: %'zu\n",
                      meminfo.usable_bytes,
                      meminfo.used_bytes,
                      fraction_to_percentage(meminfo.used_bytes, meminfo.usable_bytes),
                      meminfo.reserved_bytes);
}
