#pragma once

#include <stddef.h>
#include <stdint.h>

#define GDT_ENTRIES 6

struct gdt64_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
}
__attribute((packed));

struct gdt64_descriptor
{
    uint16_t segment;
    uint64_t offset;
}
__attribute((packed));

extern "C" void gdt_load(gdt64_descriptor* descriptor);

void gdt_initialize();
