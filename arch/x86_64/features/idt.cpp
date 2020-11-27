#include "x86_64/features/idt.h"
#include "x86_64/features/pic_8259.h"

#include "kernel/io.h"
#include "kernel/drivers/serial.h"

#define INTERRUPT_GATE 0x8e

idt64 _idt[IDT_ENTRIES] = {};
idt64_descriptor _idt_descriptor =
{
    .segment = sizeof(idt64) * IDT_ENTRIES,
    .offset = (uint64_t)&_idt,
};

extern "C" void idt_load(uint64_t descriptor);

extern uint64_t interrupt_irq1;

idt64 idt_entry_create(uint64_t offset, uint8_t ist, uint8_t types_atrr)
{
    idt64 entry;

    entry.ist = ist;
    entry.types_attr = types_atrr;
    entry.selector = 0x08;
    entry.offset_low = offset & 0xffff;
    entry.offset_mid = (offset >> 16) & 0xffff;
    entry.offset_high = (offset >> 32) & 0xffffffff;
    entry.zero = 0;

    return entry;
}

void idt_initialize()
{
    _idt[1] = idt_entry_create((uint64_t)&interrupt_irq1, 0, INTERRUPT_GATE);

    pic8259_remap();
    idt_load((uint64_t)&_idt_descriptor);
}
