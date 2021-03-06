#include "kernel/io.h"
#include "x86_64/features/pic_8259.h"
#include "kernel/drivers/serial.h"

void interrupts_enable()
{
    asm("sti");
}

void interrupts_disable()
{
    asm("cli");
}

extern "C" void irq1_handler()
{
    // stub interrupt
    uint8_t data = inb(0x60);
    serial::print(COM1, "interrupt 0x21 - ps/2 keyboard");

    pic8259_master_eoi();
}

extern "C" void double_fault_handler()
{
    serial::print(COM1, "kernel panic!\n\ndouble fault");

    while(true);
}

extern "C" void protection_fault_handler()
{
    serial::print(COM1, "kernel panic!\n\ngeneral protection fault");

    while(true);
}
