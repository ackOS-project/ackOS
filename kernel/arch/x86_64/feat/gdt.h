#pragma once

#include <liback/utils/macros.h>

#include <stddef.h>
#include <stdint.h>

#define GDT_ENTRIES 6

namespace x86_64
{
    enum
    {
        GDT_LONG_MODE_GRANULARITY = 0x20,
        GDT_SEGMENT = 0x10,
        GDT_PRESENT = 0x80,
        GDT_TSS_PRESENT = 0x80,
        GDT_USER = 0x60,
        GDT_EXECUTABLE = 0x8,
        GDT_READ_WRITE = 0x2,
        GDT_FLAGS = 0xC
    };

    struct tss_struct
    {
        uint32_t reserved0;
        uint64_t rsp[3];
        uint64_t reserved1;
        uint64_t ist[7];
        uint32_t reserved2;
        uint32_t reserved3;
        uint16_t reserved4;
        uint16_t iomap_base;
    }
    ATTRIBUTE_PACKED;

    struct tss_entry
    {
        uint16_t size = 0;
        uint16_t base_low = 0;
        uint8_t base_mid = 0;
        uint8_t flags1 = 0;
        uint8_t flags2 = 0;
        uint8_t base_high = 0;
        uint32_t base_upper = 0;
        uint32_t reserved0 = 0;
    }
    ATTRIBUTE_PACKED;

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

    struct gdt_struct
    {
        gdt_entry gdt[GDT_ENTRIES]{};
        tss_entry tss;
    }
    ATTRIBUTE_PACKED;

    static_assert(sizeof(gdt_entry) == sizeof(uint64_t));

    void tss_set_stack(uint64_t addr);

    gdt_entry gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t access);

    tss_entry gdt_create_tss_entry(uint64_t addr);

    void gdt_initialise();

    void gdt_load(gdt_descriptor* descriptor);

    void tss_flush();
}
