#include <stddef.h>
#include <stdint.h>

#include "kernel/arch/x86_64/idt.h"
#include "kernel/arch/x86_64/gdt.h"
#include "kernel/arch/x86_64/mem.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/acpi.h"
#include "kernel/lib/log.h"

#include "lib/liback/util.h"

#define GATE_INTERRUPT 0xE
#define GATE_TRAP 0xF

#define IDT_ENTRIES 256

#define IDT_HANDLER_ENTRY(HANDLER_, SELECTOR_, TYPE_, PRIV_) \
    (struct idt_entry) { \
        .offset_low = (uint16_t)((uintptr_t)HANDLER_), \
        .selector = (SELECTOR_), \
        .ist = 0, /* don't switch stacks on an ISR */ \
        .zero0 = 0, \
        .type = (TYPE_), \
        .privilege_level = (PRIV_), \
        .present = 1, \
        .offset_mid = (uint16_t)((uintptr_t)(HANDLER_) >> 16), \
        .offset_high = (uint32_t)(((uintptr_t)(HANDLER_)) >> 32), \
        .zero1 = 0 \
    }

#define IDT_INT_ENTRY(HANDLER_) IDT_HANDLER_ENTRY(HANDLER_, KERNEL64_CS, GATE_INTERRUPT, DPL0)

extern const uintptr_t idt_vect_list[];
static struct idt_entry idt_entries[IDT_ENTRIES] = { IDT_INT_ENTRY(NULL) };
static const struct idt_descriptor idt_desc = {
    .limit = sizeof(idt_entries) - 1,
    .offset = (uintptr_t)&idt_entries
};

static const char* exception_names[] =
{
    "division by zero error",
    "debug",
    "non-maskable interrupt",
    "breakpoint",
    "overflow",
    "bound range exceeded",
    "invalid opcode",
    "device not available",
    "double fault",
    "coprocessor segment overrun (deprecated)",
    "invalid TSS",
    "segment not present",
    "stack segment fault",
    "general protection fault",
    "page fault",
    "(reserved)",
    "x87 FPU error",
    "alignment check",
    "machine check",
    "SIMD floating point error",
    "virtualisation error",
    "control protection exception",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "hypervisor injection exception ",
    "VMM communication exception",
    "security exception",
    "(reserved)"
};

static inline const char* get_exception_name(int index)
{
    return index < STATIC_LEN(exception_names) ? exception_names[index] : "(unknown exception)";
}

void interrupt_handler(struct int_frame* frame)
{
    if(frame->int_num >= 32) /* external interrupt */
    {
        kprintf(KERN_DEBUG "received external interrupt %lu\n", frame->int_num);

        if(frame->int_num == 33)
        {
            uint8_t key = inb(0x60);

            kprintf(KERN_DEBUG_CONT "which is a key %s interrupt with the scancode: %hhu\n", (key & (1 << 7)) ? "release" : "press", (key & 0x7f));
        }

        lapic_eoi();

        return;
    }

    if(frame->int_num == 14) /* page fault */
    {
        if(obtain_kernel_context()->vmm_table)
        {
            vmm_print_mapping(obtain_kernel_context(), (virt_addr_t)reg_get_cr2());
        }

        kprintf(KERN_WARN "%#lx: page %s %s%s %s %s %s\n",
                          reg_get_cr2(),
                          frame->err & 1 ? "protection violation" : "not present",
                          frame->err & (1 << 1) ? "on write" : "on read",
                          frame->err & (1 << 2) ? " while in usermode" : "",
                          frame->err & (1 << 3) ? "reserved bits were set" : "",
                          frame->err & (1 << 3) && frame->err & (1 << 4) ? "and" : "",
                          frame->err & (1 << 4) ? "tried executing when the NX bit was set" : "");
    }

    kprintf(KERN_PANIC "%s INT\n"
                       "    rax:      %#lx\n"
                       "    rbx:      %#lx\n"
                       "    rcx:      %#lx\n"
                       "    rdx:      %#lx\n"
                       "    rsi:      %#lx\n"
                       "    rdi:      %#lx\n"
                       "    rbp:      %#lx\n"
                       "    r8:       %#lx\n"
                       "    r9:       %#lx\n"
                       "    r10:      %#lx\n"
                       "    r11:      %#lx\n"
                       "    r12:      %#lx\n"
                       "    r13:      %#lx\n"
                       "    r14:      %#lx\n"
                       "    r15:      %#lx\n"
                       "    int_num:  %lu\n"
                       "    err_code: %#lx\n"
                       "    rip:      %#lx\n", 
                            get_exception_name(frame->int_num),
                            frame->rax,
                            frame->rbx,
                            frame->rcx,
                            frame->rdx,
                            frame->rsi,
                            frame->rdi,
                            frame->rbp,
                            frame->r8,
                            frame->r9,
                            frame->r10,
                            frame->r11,
                            frame->r12,
                            frame->r13,
                            frame->r14,
                            frame->r15,
                            frame->int_num,
                            frame->err,
                            frame->rip);
}

static void print_idt_entry(size_t i)
{
    struct idt_entry* entry = &idt_entries[i];

    kprintf(KERN_DEBUG "idt_entry #%lu {\n"
                       "    offset:   %#lx\n"
                       "        off_low:  %#hx\n"
                       "        off_mid:  %#hx\n"
                       "        off_high: %#x\n"
                       "    selector: %#hx\n"
                       "    ist:      %#hx\n"
                       "    zero0:    %hu\n"
                       "    types_attr:  %#hx\n"
                       "       type:     %#hx\n"
                       "       priv:     %#hx\n"
                       "       present:  %hu\n"
                       "    zero1:    %u\n"
                       "}\n",
                       i,
                       ((uintptr_t)entry->offset_high << 32 | (uintptr_t)entry->offset_mid << 16 | (uintptr_t)entry->offset_low),
                       entry->offset_low,
                       entry->offset_mid,
                       entry->offset_high,
                       entry->selector,
                       entry->ist,
                       entry->zero0,
                       entry->types_attr,
                       entry->type,
                       entry->privilege_level,
                       entry->present,
                       entry->zero1);
}

static void print_idt_desc(void)
{
    kprintf(KERN_DEBUG "idt_desc {\n"
        "    limit:  %hu\n"
        "    offset: %#lx\n"
        "}\n",
        idt_desc.limit,
        idt_desc.offset);
}

void init_idt(void)
{
    for(size_t i = 0; i < IDT_ENTRIES; i++)
    {
        uintptr_t handler = idt_vect_list[i];
        struct idt_entry entry = IDT_INT_ENTRY(handler);

        idt_entries[i] = entry;
    }

/*
    // Just print the first three entries; we can assume that the rest are fine
    for(size_t i = 0; i < 3; i++)
    {
        print_idt_entry(i);
    }

    print_idt_desc();
*/

    __asm__ volatile("lidtq %0" :: "m"(idt_desc));
}

