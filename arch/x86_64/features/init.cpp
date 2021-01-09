#include "x86_64/features/CPUID.h"
#include "x86_64/features/idt.h"
#include "x86_64/features/gdt.h"
#include "x86_64/features/pic_8259.h"
#include "x86_64/features/interrupts.h"
#include "kernel/multiboot2.h"
#include "kernel/kernel.h"

void kmain(multiboot_header* header);

extern "C" int x86_64_init(multiboot_header* header, unsigned int magic)
{
    gdt_initialize();
    idt_initialize();

    pic8259_remap();
    pic8259_mask();

    interrupts_enable();

    kmain(header);

    return 0;
}
