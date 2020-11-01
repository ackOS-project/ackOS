#include "kernel/vga.h"
#include "kernel/kb.h"

void keyboard_interrupt_handler()
{
    vga_terminal::write("x");
}

void ksh_prompt()
{
    vga_terminal::write("$ ");
}

void ksh_start()
{
    vga_terminal::write("Started kernel shell\n\n");

    ksh_prompt();
}
