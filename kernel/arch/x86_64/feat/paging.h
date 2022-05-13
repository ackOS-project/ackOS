#pragma once

#include <liback/utils/macros.h>
#include <cstdint>
#include <cstddef>

namespace x86_64
{
    enum pml_entry_flag
    {
        PML_ENTRY_PRESENT,
        PML_ENTRY_WRITABLE,
        PML_ENTRY_USER_ACCESS,
        PML_ENTRY_CACHING,
        PML_ENTRY_DISABLE_CACHE,
        PML_ENTRY_ACCESSED,
        PML_ENTRY_DIRTY,
        PML_ENTRY_HUGE_PAGE,
        PML_ENTRY_GLOBAL,
        PML_ENTRY_NO_EXECUTE = 63
    };

    class pml_entry
    {
    private:
        uint64_t _raw;
    
    public:
        pml_entry()
        :
        _raw(0)
        {
        }

        pml_entry(uint64_t raw)
        :
        _raw(raw)
        {
        }

        void set(pml_entry_flag flag, bool state)
        {
            if(state)
            {
                _raw |= 1 << flag;
            }
            else
            {
                _raw &= ~(1 << flag);
            }
        }

        bool get(pml_entry_flag flag) const
        {
            return _raw & (1 << flag);
        }

        void set_present(bool state) { set(PML_ENTRY_PRESENT, state); }
        bool get_present() const { return get(PML_ENTRY_PRESENT); }

        void set_writable(bool state) { set(PML_ENTRY_WRITABLE, state); }
        bool get_writable() const { return get(PML_ENTRY_WRITABLE); }

        void set_user_access(bool state) { set(PML_ENTRY_USER_ACCESS, state); }
        bool get_user_access() const { return get(PML_ENTRY_USER_ACCESS); }

        void set_caching(bool state) { set(PML_ENTRY_CACHING, state); }
        bool get_caching() const { return get(PML_ENTRY_CACHING); }

        void set_disable_cache(bool state) { set(PML_ENTRY_DISABLE_CACHE, state); }
        bool get_disable_cache() const { return get(PML_ENTRY_DISABLE_CACHE); }

        void set_accessed(bool state) { set(PML_ENTRY_ACCESSED, state); }
        bool get_accessed() const { return get(PML_ENTRY_ACCESSED); }

        void set_dirty(bool state) { set(PML_ENTRY_DIRTY, state); }
        bool get_dirty() const { return get(PML_ENTRY_DIRTY); }

        void set_huge_page(bool state) { set(PML_ENTRY_HUGE_PAGE, state); }
        bool get_huge_page() const { return get(PML_ENTRY_HUGE_PAGE); }

        void set_global(bool state) { set(PML_ENTRY_GLOBAL, state); }
        bool get_global() const { return get(PML_ENTRY_GLOBAL); }

        void set_no_execute(bool state) { set(PML_ENTRY_NO_EXECUTE, state); }
        bool get_no_execute() const { return get(PML_ENTRY_NO_EXECUTE); }

        void set_physical_address(uint64_t addr);
        uint64_t get_physical_address() const;
    };

    struct pml_t
    {
        pml_entry entries[512];
    };

    static_assert(sizeof(pml_entry) == sizeof(uint64_t), "pml_entry must be 64 bits wide");

    void paging_switch_directory(pml_t* pd);

    void paging_flush();

    void paging_map(uintptr_t virtual_addr, uintptr_t phys_addr, uint32_t flags);

    void paging_unmap(uintptr_t virtual_addr);

    void paging_initialise();
}
