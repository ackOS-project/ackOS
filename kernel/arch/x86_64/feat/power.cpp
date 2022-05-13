#include "kernel/arch/x86_64/feat/io.h"
#include "kernel/arch/arch.h"

#define bit(n) (1 << (n))
#define check_flag(flags, n) ((flags)&bit(n))

namespace x86_64
{
    void reboot_8042()
    {
        uint8_t temp;

        arch::interrupts_disable();
        do
        {
            temp = inb(0x64); /* clear user data */

            if(check_flag(temp, 0) != 0)
            {
                inb(0x60); /* empty keyboard data */
            }
        }
        while(check_flag(temp, 1) != 0);

        outb(0x64, 0xFE); /* send CPU reset */

        arch::halt();
    }

    void shutdown_virtual_machine()
    {
        outw(0xB004, 0x2000); /* bochs */
        outw(0x604, 0x2000); /* qemu */
        outw(0x4004, 0x3400); /* virtualbox */
    }
}
