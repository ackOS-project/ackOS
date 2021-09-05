#include "kernel/arch/x86_64/features/gdt.h"
#include "kernel/logger.h"

namespace x86_64
{
    static gdt_struct _gdt;
    static tss_struct _tss = {};

    extern "C" void jump_usermode();

    void gdt_initialise()
    {
        gdt_descriptor _gdt_descriptor =
        {
            .segment = sizeof(gdt_struct) - 1,
            .offset = (uint64_t)&_gdt
        };

        _gdt.gdt[0] = gdt_create_entry(0, 0, 0, 0);

        _gdt.gdt[1] = gdt_create_entry(0, 0, GDT_LONG_MODE_GRANULARITY, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE);
        _gdt.gdt[2] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE);

        _gdt.gdt[3] = gdt_create_entry(0, 0, GDT_LONG_MODE_GRANULARITY, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);
        _gdt.gdt[4] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);

        _gdt.tss = gdt_create_tss_entry((uint64_t)&_tss);

        gdt_load(&_gdt_descriptor);
        //tss_flush();
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

    tss_entry gdt_create_tss_entry(uint64_t addr)
    {
        tss_entry entry;

        entry.size = sizeof(tss_struct);

        entry.base_low = (uint16_t)addr;
        entry.base_mid = (uint8_t)(addr >> 16);
        entry.base_high = (uint8_t)(addr >> 24);
        entry.base_upper = (uint32_t)(addr >> 32);

        entry.flags1 = 0b10001001;
        entry.flags2 = 0;

        entry.reserved0 = 0;

        return entry;
    }

    void tss_set_stack(uint64_t addr)
    {
        _tss.rsp[0] = addr;
        _tss.ist[0] = addr;
    }

    extern "C" void gdt64_load(gdt_descriptor* descriptor);
    extern "C" void tss64_flush();

    /* C++ wrapper */
    void gdt_load(gdt_descriptor* descriptor)
    {
        gdt64_load(descriptor);
    }

    void tss_flush()
    {
        tss64_flush();
    }
}
