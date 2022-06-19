#include "kernel/arch/x86_64/feat/paging.h"
#include "kernel/arch/x86_64/feat/cpuid.h"
#include "kernel/arch/x86_64/feat/asm.h"
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
    static bool level5_paging_available = false;
    static bool level5_paging_enabled = false;

    uint64_t pml_entry::get_physical_address() const
    {
        return _raw & FRAME_MASK;
    }

    void pml_entry::set_physical_address(uint64_t addr)
    {
        addr &= 0xffffffffffUL;

        _raw &= 0xfff0000000000fffUL;
        _raw |= addr;
    }

    static bool five_level_paging_check()
    {
        uint32_t unused, ecx;
        __get_cpuid(0, &unused, &unused, &ecx, &unused);

        return ecx & x86_64::cpuid::FEAT_EDX_PAT;
    }
    
    static bool five_level_paging_enabled()
    {
        return get_cr4() & (1 << 12);
    }

    static void enable_five_level_paging()
    {
        // TODO:
        // The CR4.LA57 bit cannot be set or cleared in long mode, doing so will trigger a GP fault.
        // We need to turn off long mode to set the bit.
        /*
        uint64_t cr4 = get_cr4() | 1 << 12;

        set_cr4(cr4);

        level5_paging_enabled = true;
        */
    }

    void paging_switch_table(uintptr_t table)
    {
        set_cr3(table);
    }

    pml_entry paging_create_entry(uintptr_t physical_addr, uint32_t flags)
    {
        pml_entry entry;
        
        entry.set_physical_address(physical_addr);
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
        set_cr3(get_cr3());
    }

    static pml_t* paging_get_pml_or_alloc(pml_t* table, uint64_t index, uint32_t flags)
    {
        pml_t* pml = nullptr;

        if(table->entries[index].get_present())
        {
            pml = (pml_t*)(table->entries[index].get_physical_address());
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
            pml = (pml_t*)(table->entries[index].get_physical_address());
        }

        return pml;
    }

    void paging_map(uintptr_t table, uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags)
    {
        pml_t* root_pml = (pml_t*)table;

        if(level5_paging_enabled)
        {
            root_pml = paging_get_pml_or_alloc((pml_t*)table, PML5_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        }

        pml_t* pml3 = paging_get_pml_or_alloc(root_pml, PML4_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        pml_t* pml2 = paging_get_pml_or_alloc(pml3, PML3_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);
        pml_t* pml1 = paging_get_pml_or_alloc(pml2, PML2_INDEX(virtual_addr), VIRTUAL_FLAG_USER | VIRTUAL_FLAG_WRITE);

        pml1->entries[PML1_INDEX(virtual_addr)] = paging_create_entry(phys_addr, flags);
    }

    void paging_free_table_if_empty(pml_t* table, pml_t* parent, uint64_t index)
    {
        for(int i = 0; i < 512; i++)
        {
            if(table->entries[i].get_present())
            {
                return;
            }
        }

        physical_deallocate(table, 1);
        parent->entries[index].set_present(false);
    }

    void paging_unmap(uintptr_t table, uintptr_t virtual_addr)
    {
        pml_t* root_pml = (pml_t*)table;

        if(level5_paging_enabled)
        {
            root_pml = paging_get_pml((pml_t*)table, PML5_INDEX(virtual_addr));
        }

        pml_t* pml3 = paging_get_pml(root_pml, PML4_INDEX(virtual_addr));
        pml_t* pml2 = paging_get_pml(pml3, PML3_INDEX(virtual_addr));
        pml_t* pml1 = paging_get_pml(pml2, PML2_INDEX(virtual_addr));

        pml1->entries[PML1_INDEX(virtual_addr)] = pml_entry(0);

        paging_free_table_if_empty(pml1, pml2, PML2_INDEX(virtual_addr));
        paging_free_table_if_empty(pml2, pml3, PML3_INDEX(virtual_addr));
        paging_free_table_if_empty(pml3, root_pml, PML4_INDEX(virtual_addr));

        if(level5_paging_enabled)
        {
            paging_free_table_if_empty(root_pml, (pml_t*)table, PML5_INDEX(virtual_addr));
        }
    }

    uintptr_t paging_table_create()
    {
        level5_paging_available = five_level_paging_check();
        level5_paging_enabled = five_level_paging_enabled();

        return (uintptr_t)physical_allocate_clear(1);
    }
}
