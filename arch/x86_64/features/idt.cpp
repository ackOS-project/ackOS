#include "arch/x86_64/features/idt.h"
#include "arch/x86_64/features/gdt.h"
#include "arch/x86_64/features/pic_8259.h"
#include "arch/x86_64/features/io.h"

#include "kernel/logger.h"
#include "arch/x86_64/features/com.h"

#define INTERRUPT_GATE32 0x8E
#define TRAP_GATE32 0xEF
#define INTERRUPT_USER 0x60

extern uint64_t _interrupt_vt[];

namespace x86_64
{
    static idt_entry idt[IDT_ENTRIES];

    idt_entry idt_entry_create(uint64_t handler, uint8_t ist, uint8_t types_atrr)
    {
        idt_entry entry =
        {
            .offset_low = (uint16_t)handler,
            .selector = 0x08,
            .ist = ist,
            .types_attr = types_atrr,
            .offset_mid = (uint16_t)(handler >> 16),
            .offset_high = (uint32_t)(handler >> 32),
            .zero = 0
        };

        return entry;
    }

    void idt_load(idt_descriptor* descriptor)
    {
        asm volatile("lidtq %0" :: "m" (*descriptor));
    }

    void idt_initialise()
    {
        idt_descriptor idt_descriptor =
        {
            .limit = (sizeof(idt_entry) * IDT_ENTRIES) - 1,
            .offset = (uint64_t)&idt,
        };

        for(int i = 0; i < 48; i++)
        {
            idt[i] = idt_entry_create(_interrupt_vt[i], 0, INTERRUPT_GATE32);
        }

        idt_load(&idt_descriptor);
    }
}
