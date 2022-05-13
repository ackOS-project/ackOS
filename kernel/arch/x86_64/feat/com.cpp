#include "kernel/arch/x86_64/feat/com.h"
#include "kernel/arch/x86_64/feat/io.h"

#include <cstring>
#include <cstdlib>

namespace x86_64
{
    void com_initialize(int addr, int baud_rate)
    {
        x86_64::outb(addr + 1, 0x00); /* disable interrupts */

        x86_64::outb(addr + 3, 0x80); /* set baud rate divisor */
        x86_64::outb(addr + 0, 115200 / baud_rate); /* ^ low byte */
        x86_64::outb(addr + 1, 0x00);               /* high byte */

        x86_64::outb(addr + 3, 0x03); /* 8 bits */
        x86_64::outb(addr + 2, 0x00); /* enable FIFO with 14-byte threshold */
        x86_64::outb(addr + 4, 0x01); /* IRQ, RTS/DSR set */
    }

    static int transmit_empty(int addr)
    {
        return x86_64::inb(addr + 5) & 0x20;
    }

    void com_putc(int addr, char c)
    {
        while(transmit_empty(addr) == 0);

        x86_64::outb(addr, c);
    }
 
    char com_getc(int addr)
    {
        while((x86_64::inb(addr + 5) & 1) == 0);
 
        return x86_64::inb(addr);
    }

    void com_write(int addr, const char* s, size_t size)
    {
        for(int i = 0; i < size; i++)
        {
            com_putc(addr, s[i]);
        }
    }
} 
