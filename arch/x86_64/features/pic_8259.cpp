/*
pic_8259.cpp: For legacy PIC controllers
*/

#include "x86_64/features/pic_8259.h"
#include "kernel/io.h"

void pic8259_remap()
{
    outb(PIC1_DATA, 0xfd);
    outb(PIC2_DATA, 0xff);
}

void pic8259_finish_interrupt()
{
    outb(PIC1, PIC1);
    outb(PIC2, PIC1);
}
