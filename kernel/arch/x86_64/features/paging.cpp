#include "kernel/arch/x86_64/features/paging.h"

#define FRAME_SIZE 4096
#define FRAME_MASK 0xfffffffffffff000

namespace x86_64
{
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

    inline void paging_load_directory(uint64_t pd)
    {
        asm volatile("mov cr3, %0" ::"r"(pd));
    }

    uint64_t paging_get_page_mask()
    {
        return FRAME_MASK;
    }

    uint64_t paging_get_page_size()
    {
        return FRAME_SIZE;
    }
}
