#include "x86_64/features/idt.h"
#include "x86_64/features/gdt.h"
#include "x86_64/features/pic_8259.h"

#include "kernel/io.h"
#include "kernel/drivers/serial.h"

#define INTERRUPT_GATE 0x8e

idt64_entry _idt[IDT_ENTRIES];

extern uint64_t interrupt_irq1;

extern gdt64_entry _gdt[GDT_ENTRIES];

idt64_entry idt_entry_create(uint64_t offset, uint8_t ist, uint8_t types_atrr)
{
    idt64_entry entry;

    entry.ist = ist;
    entry.types_attr = types_atrr;
    entry.selector = 0x08;
    entry.offset_low = offset & 0xffff;
    entry.offset_mid = (offset & 0xffff) >> 16;
    entry.offset_high = (offset & 0xffffffff) >> 32;
    entry.zero = 0;

    return entry;
}

void idt_initialize()
{
    _idt[1] = idt_entry_create((uint64_t)&interrupt_irq1, 0, INTERRUPT_GATE);

    idt64_descriptor _idt_descriptor =
    {
        .segment = sizeof(idt64_entry) * IDT_ENTRIES,
        .offset = (uint64_t)&_idt,
    };

    idt_load(&_idt_descriptor);
}
