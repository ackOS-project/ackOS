#pragma once

#include <liback/utils/macros.h>
#include <cstdint>
#include <cstddef>

namespace x86_64
{
    enum page_table_flag
    {
        PT_PRESENT,
        PT_WRITABLE,
        PT_USER_ACCESS,
        PT_CACHING,
        PT_CACHE_DISABLE,
        PT_ACCESSED,
        PT_DIRTY,
        PT_HUGE_PAGE,
        PT_GLOBAL,
        PT_NO_EXECUTE = 63
    };

    class page_table
    {
    private:
        uint64_t _pt;

    public:
        page_table(uint64_t value)
        :
        _pt(value)
        {
        }

        page_table()
        :
        _pt(0)
        {
        }

        void set_value(uint64_t value) { _pt = value; }
        uint64_t get_value() const { return _pt; }

        uint64_t get_address() const;

        bool get_flag(page_table_flag flag) const;
        void set_flag(page_table_flag flag, bool value);
    };

    static_assert(sizeof(page_table) == sizeof(uint64_t));

    void paging_load_directory(uint64_t pd);

    uint64_t paging_get_page_mask();

    uint64_t paging_get_page_size();

}
