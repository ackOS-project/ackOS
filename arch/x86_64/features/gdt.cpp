#include "arch/x86_64/features/gdt.h"
#include "kernel/logger.h"
#include <cstdio>

namespace x86_64
{
    static gdt_entry _gdt[GDT_ENTRIES];

    void gdt_initialise()
    {
        gdt_descriptor _gdt_descriptor =
        {
            .segment = (sizeof(gdt_entry) * GDT_ENTRIES) - 1,
            .offset = (uint64_t)&_gdt
        };

        _gdt[0] = gdt_create_entry(0, 0, 0, 0);
        _gdt[1] = gdt_create_entry(0, 0, 0x20, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE);
        _gdt[2] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE);
        _gdt[3] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);
        _gdt[4] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);

        gdt_load(&_gdt_descriptor);
    }

    gdt_entry gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access)
    {
        gdt_entry entry;

        entry.base_low = (base & 0xFFFF);
        entry.base_mid = (base >> 16) & 0xFF;
        entry.base_high = (base >> 24) & 0xFF;

        entry.limit_low = (limit & 0xFFFF);
        entry.granularity = (limit >> 16) & 0x0F;

        entry.granularity |= granularity & 0xF0;
        entry.access = access;

        return entry;
    }

    extern "C" void gdt64_load(gdt_descriptor* descriptor);

    /* C++ wrapper */
    void gdt_load(gdt_descriptor* descriptor)
    {
        gdt64_load(descriptor);
    }
}
