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

idt64_descriptor _idt_descriptor =
{
    .segment = (sizeof(idt64_entry) * IDT_ENTRIES) - 1,
    .offset = (uint64_t)&_idt,
};

void idt_entry_create(uint8_t index, uint64_t handler, uint8_t ist, uint8_t types_atrr)
{
    _idt[index] =
    {
        .offset_low = (uint16_t)handler,
        .selector = 0x08,
        .ist = ist,
        .types_attr = types_atrr,
        .offset_mid = (uint16_t)(handler >> 16),
        .offset_high = (uint32_t)(handler >> 32),
        .zero = 0
    };
}

void idt_load(idt64_descriptor descriptor)
{
    asm volatile("lidtq %0" :: "m" (descriptor));
}

void idt_initialize()
{
    idt_entry_create(0x8, (uint64_t)&interrupt_double_fault, 0, INTERRUPT_GATE32);
    idt_entry_create(0xD, (uint64_t)&interrupt_protection_fault, 0, INTERRUPT_GATE32);
    idt_entry_create(0x21, (uint64_t)&interrupt_irq1, 0, INTERRUPT_GATE32);

    idt_load(_idt_descriptor);
}
