#pragma once

namespace x86_64
{
    enum
    {
        COM1 = 0x3f8,
        COM2 = 0x2F8,
        COM3 = 0x3E8,
        COM4 = 0x2E8
    };

    void com_initialize(int addr, int baud_rate);

    void com_putc(int addr, char c);

    char com_getc(int addr);
}
