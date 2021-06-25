#pragma once

#include <liback/utils/macros.h>
#include <stddef.h>
#include <stdint.h>

#define GDT_ENTRIES 6

namespace x86_64
{
    enum
    {
        GDT_LONG_MODE_GRANULARITY = 0x2,
        GDT_SEGMENT = 0x10,
        GDT_PRESENT = 0x80,
        GDT_TSS_PRESENT = 0x80,
        GDT_USER = 0x60,
        GDT_EXECUTABLE = 0x8,
        GDT_READ_WRITE = 0x2,
        GDT_FLAGS = 0xC
    };

    struct gdt_entry
    {
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high;
    }
    ATTRIBUTE_PACKED;

    struct gdt_descriptor
    {
        uint16_t segment;
        uint64_t offset;
    }
    ATTRIBUTE_PACKED;

    gdt_entry gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access);

    void gdt_initialise();

    void gdt_load(gdt_descriptor* descriptor);
}
