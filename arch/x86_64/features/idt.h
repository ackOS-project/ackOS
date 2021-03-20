#pragma once

#include <stdint.h>
#include <stddef.h>

#define IDT_ENTRIES 256

struct idt64_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t types_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

struct idt64_descriptor
{
    uint16_t segment;
    uint64_t offset;
}
__attribute((packed));

void idt_load(idt64_descriptor descriptor);

void idt_initialize();
