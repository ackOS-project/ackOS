#include "kernel/arch/x86_64/features/paging.h"
#include "kernel/arch/x86_64/features/cpuid.h"
#include "kernel/arch/x86_64/features/instructions.h"
#include "kernel/mm/physical.h"
#include "kernel/mm/virtual.h"
#include "kernel/sys/logger.h"

#include <cstring>
#include <liback/utils/assert.h>

#define FRAME_SIZE 4096
#define FRAME_MASK 0xfffffffffffff000UL

#define PML5_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 48)) >> 48)
#define PML4_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 39)) >> 39)
#define PML3_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 30)) >> 30)
#define PML2_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 21)) >> 21)
#define PML1_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 12)) >> 12)

namespace x86_64
{
    static pml_t* kernel_pml = nullptr;
    static bool level5_paging_available = false;

    uint64_t pml_entry::get_physical_address() const
    {
        return (_raw & FRAME_MASK) >> 12;
    }

    void pml_entry::set_physical_address(uint64_t addr)
    {
        addr &= 0xffffffffffUL;

        _raw &= 0xfff0000000000fffUL;
        _raw |= addr << 12;
    }

    static bool five_level_paging_check()
    {
        uint32_t unused, ecx;
        __get_cpuid(0, &unused, &unused, &ecx, &unused);

        return (ecx & x86_64::cpuid::FEAT_EDX_PAT) && (get_cr4() & 1 << 12);
    }

    static void enable_five_level_paging()
    {
        // TODO:
        // The CR4.LA57 bit cannot be set or cleared in long mode, doing so will trigger a GP fault.
        // We need to turn off long mode to set the bit.
        /*
        uint64_t cr4 = get_cr4() | 1 << 12;

        set_cr4(cr4);
        */
    }

    void paging_switch_directory(pml_t* pml)
    {
        set_cr3((uint64_t)pml);

        if(level5_paging_available)
        {
            enable_five_level_paging();
        }
    }

    pml_entry paging_create_entry(uintptr_t physical_addr, uint32_t flags)
    {
        pml_entry entry;

        entry.set_physical_address(physical_addr >> 12);
        entry.set_present(true);
        entry.set_writable(flags & VIRTUAL_FLAG_WRITE);
        entry.set_user_access(flags & VIRTUAL_FLAG_USER);
        entry.set_caching(false);
        entry.set_disable_cache(false);
        entry.set_accessed(false);
        entry.set_dirty(false);
        entry.set_huge_page(false);
        entry.set_global(false);
        entry.set_no_execute(false);

        return entry;
    }

    void paging_flush()
    {
        paging_switch_directory(kernel_pml);
    }

    static bool is_pml_empty(pml_t* table)
    {
        for(int i = 0; i < 512; i++)
        {
            if(table->entries[i].get_present())
            {
                return false;
            }
        }

        return true;
    }

    static pml_t* paging_get_pml_or_alloc(pml_t* table, uint64_t index, uint32_t flags)
    {
        pml_t* pml = nullptr;

        if(table->entries[index].get_present())
        {
            pml = (pml_t*)(table->entries[index].get_physical_address() << 12);
        }
        else
        {
            pml = (pml_t*)physical_allocate_clear(1);
            table->entries[index] = paging_create_entry((uintptr_t)pml, flags);
        }

        return pml;
    }

    static pml_t* paging_get_pml(pml_t* table, uint64_t index)
    {
        pml_t* pml = nullptr;

        if(table->entries[index].get_present())
        {
            pml = (pml_t*)(table->entries[index].get_physical_address() << 12);
        }

        return pml;
    }

    void paging_map(pml_t* table, uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags)
    {
        pml_t* root_pml = table;

        if(level5_paging_available)
        {
            root_pml = paging_get_pml_or_alloc(table, PML5_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        }

        pml_t* pml3 = paging_get_pml_or_alloc(root_pml, PML4_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        pml_t* pml2 = paging_get_pml_or_alloc(pml3, PML3_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        pml_t* pml1 = paging_get_pml_or_alloc(pml2, PML2_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);

        pml_entry* entry = &pml1->entries[PML1_INDEX(virtual_addr)];

        if(!entry->get_present())
        {
            *entry = paging_create_entry(phys_addr, flags);
        }
    }

    void paging_map(uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags)
    {
        paging_map(kernel_pml, virtual_addr, phys_addr, flags);
    }

    void paging_unmap(pml_t* table, uintptr_t virtual_addr)
    {
        pml_t* root_pml = table;

        if(level5_paging_available)
        {
            root_pml = paging_get_pml(table, PML5_INDEX(virtual_addr));
        }

        pml_t* pml3 = paging_get_pml(root_pml, PML4_INDEX(virtual_addr));
        pml_t* pml2 = paging_get_pml(pml3, PML3_INDEX(virtual_addr));
        pml_t* pml1 = paging_get_pml(pml2, PML2_INDEX(virtual_addr));

        pml1->entries[PML1_INDEX(virtual_addr)] = pml_entry(0);
    }

    void paging_unmap(uintptr_t virtual_addr)
    {
        paging_unmap(kernel_pml, virtual_addr);
    }

    void paging_initialise()
    {
        kernel_pml = (pml_t*)physical_allocate_clear(1);

        if(five_level_paging_check())
        {
            level5_paging_available = true;
        }
    }
}
