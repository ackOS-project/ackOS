#include "kernel/arch/x86_64/features/paging.h"

#define FRAME_SIZE 4096
#define FRAME_MASK 0xfffffffffffff000

#define PML5_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 48)) >> 38)
#define PML4_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 39)) >> 39)
#define PML3_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 30)) >> 30)
#define PML2_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 21)) >> 21)
#define PML1_INDEX(__addr) ((__addr & ((uint64_t)0x1ff << 12)) >> 12)

namespace x86_64
{
    static uint64_t _kernel_pml4[512] ATTRIBUTE_ALIGNED(FRAME_SIZE);

    uint64_t page_table::get_address() const
    {
        return _pt & FRAME_MASK;
    }

    bool page_table::get_flag(page_table_flag flag) const
    {
        return _pt & (1 << flag);
    }

    void page_table::set_flag(page_table_flag flag, bool value)
    {
        if(value)
        {
            _pt |= 1 << flag;
        }
        else
        {
            _pt &= ~(1 << flag);
        }
    }

    void paging_switch_directory(uint64_t pd)
    {
        asm volatile("mov %0, %%cr3" ::"r"(pd));
    }

    uint64_t paging_get_page_mask()
    {
        return FRAME_MASK;
    }

    uint64_t paging_get_page_size()
    {
        return FRAME_SIZE;
    }

    void paging_initialise()
    {
        page_table entry;

        entry.set_flag(PT_PRESENT, true);
        entry.set_flag(PT_USER_ACCESS, false);
        entry.set_flag(PT_WRITABLE, true);

        uint64_t* pml4 = &_kernel_pml4[0];
        *pml4 = entry.get_value();

        paging_switch_directory((uint64_t)pml4);
    }
}
