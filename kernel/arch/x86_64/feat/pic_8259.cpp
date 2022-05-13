/*
pic_8259.cpp: For legacy PIC controllers
*/
#include <stdint.h>
#include <stddef.h>

#include "kernel/arch/x86_64/feat/pic_8259.h"
#include "kernel/arch/x86_64/feat/io.h"

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

namespace x86_64
{
    void pic8259_remap()
    { 
        uint8_t a1, a2;

        a1 = inb(PIC_MASTER_DATA);
        io_wait();
        a2 = inb(PIC_SLAVE_DATA);
        io_wait();

        outb(PIC_MASTER_COMMAND, ICW1_INIT | ICW1_ICW4);
        io_wait();
        outb(PIC_SLAVE_COMMAND, ICW1_INIT | ICW1_ICW4);
        io_wait();

        outb(PIC_MASTER_DATA, 0x20);
        io_wait();
        outb(PIC_SLAVE_DATA, 0x28);
        io_wait();

        outb(PIC_MASTER_DATA, 4);
        io_wait();
        outb(PIC_SLAVE_DATA, 2);
        io_wait();

        outb(PIC_MASTER_DATA, ICW4_8086);
        io_wait();
        outb(PIC_SLAVE_DATA, ICW4_8086);
        io_wait();

        outb(PIC_MASTER_DATA, a1);
        io_wait();
        outb(PIC_SLAVE_DATA, a2);
        io_wait();
    }

    void pic8259_mask()
    {
        outb(PIC_MASTER_DATA, 0xfd);
        outb(PIC_SLAVE_DATA, 0xff);
    }

    void pic8259_disable()
    {
        outb(PIC_MASTER_DATA, 0xff);
        outb(PIC_SLAVE_DATA, 0xff);
    }

    void pic8259_master_eoi()
    {
        outb(PIC_MASTER_COMMAND, PIC_EOI);
    }

    void pic8259_slave_eoi()
    {
        outb(PIC_SLAVE_COMMAND, PIC_EOI);
        outb(PIC_MASTER_COMMAND, PIC_EOI);
    }
}
