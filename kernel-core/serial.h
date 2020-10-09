#pragma once
#include "io.h"
#define COM1 0x3f8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void initialize_serial_port(int port, int baud_rate)
{
    outb(port + 1, 0x00); /* Disable interrupts */

    outb(port + 3, 0x80); /* Set baud rate divisor */
    outb(port + 0, 115200 / baud_rate); /* ^ low byte */
    outb(port + 1, 0x00); /* high byte */

    outb(port + 3, 0x03); /* 8 bits */
    outb(port + 2, 0x00); /* Enable FIFO with 14-byte threshold */
    outb(port + 4, 0x01); /* IRQ, RTS/DSR set */
}

int serial_transmit_empty(int port)
{
   return inb(port + 5) & 0x20;
}
 
void serial_write(int port, char a)
{
   while (serial_transmit_empty(port) == 0);
 
   outb(port, a);
}