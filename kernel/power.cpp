#include "kernel/io.h"

#define bit(n) (1 << (n))
#define check_flag(flags, n) ((flags)&bit(n))

void reboot()
{
    uint8_t temp;

    asm volatile("cli"); /* disable interrupts */
    do
    {
        temp = inb(0x64); /* clear user data */
        if(check_flag(temp, 0) != 0)
        {
            inb(0x60); /* empty keyboard data */
        }
    } while (check_flag(temp, 1) != 0);

    outb(0x64, 0xFE); /* send CPU reset */

    while(true)
    {
        asm volatile("hlt"); /* Halt the CPU */
    }
}

void shutdown()
{
    // Shutdown registers in qemu and virtual box
    outw(0xB004, 0x2000);
    outw(0x604, 0x2000);
    outw(0x4004, 0x3400);
}
