#include "x86_64/features/gdt.h"
#include "kernel/logger.h"
#include "kernel/drivers/serial.h"
#include <cstdio>

#define GDT_LONG_MODE_GRANULARITY 0x2
#define GDT_SEGMENT 0x10
#define GDT_PRESENT 0x80
#define GDT_TSS_PRESENT 0x80
#define GDT_USER 0x60
#define GDT_EXECUTABLE 0x8
#define GDT_READ_WRITE 0x2
#define GDT_FLAGS 0xC

gdt64_entry _gdt[GDT_ENTRIES];

gdt64_entry gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access)
{
    gdt64_entry entry;

    entry.base_low = (base & 0xFFFF);
    entry.base_mid = (base >> 16) & 0xFF;
    entry.base_high = (base >> 24) & 0xFF;

    entry.limit_low = (limit & 0xFFFF);
    entry.granularity = (limit >> 16) & 0x0F;

    entry.granularity |= granularity & 0xF0;
    entry.access = access;

    /*log_info("gdt", "base_low=0x%x base_mid=0x%x base_high=0x%x, limit_low=0x%x, granularity=0x%x, access=0x%x",
                    entry.base_low, entry.base_mid, entry.base_high, entry.limit_low, entry.granularity, entry.access);*/

    return entry;
}

void gdt_initialize()
{
    _gdt[0] = gdt_create_entry(0, 0, 0, 0);
    _gdt[1] = gdt_create_entry(0, 0, 0x20, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE);
    _gdt[2] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE);
    _gdt[3] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE);
    _gdt[4] = gdt_create_entry(0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);

    gdt64_descriptor _gdt_descriptor =
    {
        .segment = (sizeof(gdt64_entry) * GDT_ENTRIES) - 1,
        .offset = (uint64_t)&_gdt
    };

    gdt_load(&_gdt_descriptor);
}
