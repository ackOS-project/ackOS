#include "kernel/io.h"
#include "x86_64/features/pic_8259.h"

extern "C" void irq1_handler()
{
    // stub function

    pic8259_finish_interrupt();
}
