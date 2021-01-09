/*
pic_8259.cpp: For legacy PIC controllers
*/
#include <stdint.h>
#include <stddef.h>

#include "x86_64/features/pic_8259.h"
#include "kernel/io.h"

void pic8259_remap()
{ 
    uint8_t a1 = inb(MASTER_PIC_DATA), a2 = inb(SLAVE_PIC_DATA);
 
    outb(MASTER_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(SLAVE_PIC_COMMAND, ICW1_INIT | ICW1_ICW4);

    outb(MASTER_PIC_DATA, 0);
    outb(SLAVE_PIC_DATA, 8);

    outb(MASTER_PIC_DATA, 4);
    outb(SLAVE_PIC_DATA, 2);

    outb(MASTER_PIC_DATA, ICW1_8086);
    outb(SLAVE_PIC_DATA, ICW1_8086);


    outb(MASTER_PIC_DATA, a1);
    outb(SLAVE_PIC_DATA, a2);
}

void pic8259_mask()
{
    outb(MASTER_PIC_DATA, 0xfd);
    outb(SLAVE_PIC_DATA, 0xff);
}

void pic8259_disable()
{
    outb(MASTER_PIC_DATA, 0xff);
    outb(SLAVE_PIC_DATA, 0xff);
}

void pic8259_finish_interrupt()
{
    outb(MASTER_PIC, MASTER_PIC);
    outb(SLAVE_PIC, MASTER_PIC);
}
