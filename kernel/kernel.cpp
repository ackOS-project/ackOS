/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL licence
******************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "kernel/kernel.h"
#include <cstdio>
#include <vector>

void ksh_start();

void kmain()
{
    vga_terminal::vga_terminal(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    serial::port_initialize(COM1, 9600);

    serial::write(COM1, "Hello world!\n");

    vga_terminal::write("Hello\n");
    vga_terminal::write("Booted ackOS! meow.\n");

    ksh_start();
}
