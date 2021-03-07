#include "x86_64/features/idt.h"
#include "x86_64/features/gdt.h"
#include "x86_64/features/pic_8259.h"

#include "kernel/io.h"
#include "kernel/drivers/serial.h"

#define INTERRUPT_GATE32 0x8E
#define TRAP_GATE32 0xEF

idt64_entry _idt[IDT_ENTRIES];

extern uint64_t interrupt_irq1;
extern uint64_t interrupt_double_fault;
extern uint64_t interrupt_protection_fault;

extern gdt64_entry _gdt[GDT_ENTRIES];

idt64_descriptor _idt_descriptor =
{
    .segment = sizeof(idt64_entry) * IDT_ENTRIES - 1,
    .offset = (uint64_t)&_idt,
};

void idt_entry_create(uint8_t offset, uint64_t handler, uint8_t ist, uint8_t types_atrr)
{
    _idt[offset].ist = ist;
    _idt[offset].types_attr = types_atrr;
    _idt[offset].selector = 0x08;
    _idt[offset].offset_low = handler & 0xffff;
    _idt[offset].offset_mid = (handler & 0xffff) >> 16;
    _idt[offset].offset_high = (handler & 0xffffffff) >> 32;
    _idt[offset].zero = 0;
}

void idt_initialize()
{
    idt_entry_create(0x8, (uint64_t)&interrupt_double_fault, 0, INTERRUPT_GATE32);
    idt_entry_create(0xD, (uint64_t)&interrupt_protection_fault, 0, INTERRUPT_GATE32);
    idt_entry_create(0x21, (uint64_t)&interrupt_irq1, 0, INTERRUPT_GATE32);

    idt_load(&_idt_descriptor);
}
