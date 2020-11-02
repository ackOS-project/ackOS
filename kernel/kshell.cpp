#include "kernel/legacy_vga.h"
#include "kernel/kb.h"
#include "kernel/serial.h"

void keyboard_interrupt_handler()
{
    legacy_vga::write("x");
}

void ksh_prompt()
{
    legacy_vga::write("$ ");
    serial::print(COM1, "$ ");

    for(;;)
    {
        char c = serial::getc(COM1);

        serial::putc(COM1, c);
    }
}

void ksh_start()
{
    legacy_vga::write("Started kernel shell\n\n");

    ksh_prompt();
}
