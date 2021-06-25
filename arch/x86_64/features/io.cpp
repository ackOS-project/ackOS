#include "arch/x86_64/features/io.h"

namespace x86_64
{
    uint8_t inb(uint16_t port)
    {
        unsigned char rv;
        __asm__ __volatile__("inb %1, %0"
                             : "=a"(rv)
                             : "dN"(port));
        return rv;
    }

    void outb(uint16_t port, uint8_t data)
    {
        __asm__ __volatile__("outb %1, %0"
                             :
                             : "dN"(port), "a"(data));
    }

    void outw(uint16_t port, uint16_t data)
    {
        asm volatile("outw %%ax, %%dx"
                     :
                     : "d"(port), "a"(data));
    }

    void io_wait()
    {
        asm volatile ("jmp 1f\n\t"
                      "1:jmp 2f\n\t"
                      "2:");
    }
}
