#pragma once

#include <liback/utils/macros.h>
#include <stdint.h>
#include <stddef.h>

#define IDT_ENTRIES 256

namespace x86_64
{
    struct idt_entry
    {
        uint16_t offset_low;
        uint16_t selector;
        uint8_t ist;
        uint8_t types_attr;
        uint16_t offset_mid;
        uint32_t offset_high;
        uint32_t zero;
    };

    struct idt_descriptor
    {
        uint16_t limit;
        uint64_t offset;
    }
    ATTRIBUTE_PACKED;

    void idt_initialise();

    idt_entry idt_entry_create(uint64_t handler, uint8_t ist, uint8_t types_atrr);

    void idt_load(idt_descriptor* descriptor);
}
