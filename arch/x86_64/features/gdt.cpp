#include <x86_64/features/gdt.h>

#define GDT_LONG_MODE_GRANULARITY 2
#define GDT_SEGMENT 0b10000
#define GDT_PRESENT 0b10000000
#define GDT_TSS_PRESENT 0b10000000
#define GDT_USER 0b01100000
#define GDT_EXECUTABLE 0b00001000
#define GDT_READ_WRITE 0b00000010
#define GDT_FLAGS 0b1100

gdt64_entry _gdt[GDT_ENTRIES];

gdt64_entry gdt_create_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    gdt64_entry entry;

    entry.base_low = (base & 0xFFFF);
    entry.base_mid = (base >> 16) & 0xFF;
    entry.base_high = (base >> 24) & 0xFF;

    entry.limit_low = (limit & 0xFFFF);
    entry.granularity = (limit >> 16) & 0x0F;

    entry.granularity |= granularity & 0xF0;
    entry.access = access;

    return entry;
}

void gdt_initialize()
{
    _gdt[0] = { 0, 0, 0, 0x00, 0x00, 0 };
    _gdt[1] = { 0, 0, 0, 0x9a, 0xa0, 0 };
    _gdt[2] = { 0, 0, 0, 0x92, 0xa0, 0 };
    _gdt[3] = { 0, 0, 0, 0x00, 0x00, 0 };
    _gdt[4] = { 0, 0, 0, 0x9a, 0xa0, 0 };
    _gdt[5] = { 0, 0, 0, 0x92, 0xa0, 0 };

    gdt64_descriptor _gdt_descriptor =
    {
        .segment = (sizeof(gdt64_entry) * GDT_ENTRIES) - 1,
        .offset = (uint64_t)&_gdt
    };

    gdt_load(&_gdt_descriptor);
}
