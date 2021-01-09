#include "kernel/io.h"
#include "x86_64/features/pic_8259.h"

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

    pic8259_finish_interrupt();
}
