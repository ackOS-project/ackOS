#include "arch/x86_64/features/CPUID.h"
#include "arch/x86_64/features/idt.h"
#include "arch/x86_64/features/gdt.h"
#include "arch/x86_64/features/pic_8259.h"
#include "arch/x86_64/features/interrupts.h"
#include "arch/x86_64/features/serial.h"
#include "kernel/boot_protocols/multiboot2.h"

void kmain(void*, uint32_t);

extern "C" int x86_64_init(void* header, uint32_t magic)
{
    serial::port_initialize(SERIAL_COM1, 9600);

    gdt_initialise();
    idt_initialise();

    pic8259_remap();
    pic8259_mask();

    interrupts_enable();

    kmain(header, magic);

    return 0;
}
