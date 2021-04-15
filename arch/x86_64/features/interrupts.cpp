#include "x86_64/features/interrupts.h"
#include "kernel/io.h"
#include "x86_64/features/pic_8259.h"
#include "kernel/drivers/serial.h"
#include <cstdio>

void interrupts_enable()
{
    asm("sti");
}

void interrupts_disable()
{
    asm("cli");
}

extern "C" void interrupts_handle(interrupt_stack_frame* frame)
{
    printf("int_num=0x%x\n", frame->int_num);
}

extern "C" void irq1_handler()
{
    // stub interrupt
    uint8_t data = inb(0x60);

    serial::print(SERIAL_COM1, "interrupt 0x21 - ps/2 keyboard\n");

    pic8259_master_eoi();
}

extern "C" void double_fault_handler()
{
    serial::print(SERIAL_COM1, "kernel panic! - double fault\n");

    while(true);
}

extern "C" void protection_fault_handler()
{
    serial::print(SERIAL_COM1, "kernel panic! - general protection fault\n");

    while(true);
}
