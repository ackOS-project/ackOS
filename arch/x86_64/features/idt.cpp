#include "arch/x86_64/features/idt.h"
#include "arch/x86_64/features/gdt.h"
#include "arch/x86_64/features/pic_8259.h"

#include "kernel/io.h"
#include "kernel/logger.h"

#define INTERRUPT_GATE32 0x8E
#define TRAP_GATE32 0xEF
#define INTERRUPT_USER 0x60

extern uint64_t _interrupt_vt[];

idt64_entry _idt[IDT_ENTRIES];
idt64_descriptor _idt_descriptor =
{
    .limit = (sizeof(idt64_entry) * IDT_ENTRIES) - 1,
    .offset = (uint64_t)&_idt,
};

void idt64_entry_create(uint8_t index, uint64_t handler, uint8_t ist, uint8_t types_atrr)
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

/*
    log_info("idt", "new idt entry for 0x%x - offset_low=0x%x offset_mid=0x%x offset_high=0x%x, selector=0x%x, ist=0x%x", 
            index, _idt[index].offset_low, _idt[index].offset_mid, _idt[index].offset_high, _idt[index].selector, _idt[index].ist);
*/
}

void idt64_load(idt64_descriptor descriptor)
{
    asm volatile("lidtq %0" :: "m" (descriptor));
}

void idt_initialise()
{
    for(int i = 0; i < 48; i++)
    {
        idt64_entry_create(i, _interrupt_vt[i], 0, INTERRUPT_GATE32);
    }

    idt64_load(_idt_descriptor);
}
