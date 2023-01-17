#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/com.h"

void init_com(int port, int baud_rate)
{
    outb(port + 1, 0); /* mask interrupts */

    uint16_t baud_rate_value = 115200 / baud_rate;

    outb(port + 3, 0x80); /* set DLB bit to allow modification of the baud rate */
    outb(port + 0, (uint8_t)baud_rate_value); /* low byte */
    outb(port + 1, (uint8_t)(baud_rate_value & 0xFF00)); /* high byte */

    outb(port + 3, 0x03); /* no parity, one stop bit */
    outb(port + 2, 0); /* enable FIFO with 14-byte threshold */
    outb(port + 4, 0x1); /* enable IRQ, RTS/DSR */
}

inline void com_send_char(int port, char c)
{
    while((inb(port + 5) & 0x20) == 0);

    outb(port, c);
}

char com_receive_char(int port)
{
    while((inb(port + 5) & 0x1) == 0);

    return inb(port);
}

void com_write(int port, const char* s, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        com_send_char(port, s[i]);
    }
}
