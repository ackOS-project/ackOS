/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL licence
******************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

#include "kernel/kernel.h"
#include <cstdio>
#include <vector>

void kmain()
{
    vga_terminal::vga_terminal(VGA_COLOUR_BLACK, VGA_COLOUR_WHITE);
    serial::port_initialize(COM1, 9600);

    serial::write(COM1, "Hello world!\n");

    vga_terminal::write("Hello");
    vga_terminal::write("Booted ackOS! meow.\n");

    for(size_t i = 0; i < 10; i++)
    {
        vga_terminal::set_colour(VGA_COLOUR_RED, VGA_COLOUR_RED);
        vga_terminal::write(" ");
    }

    vga_terminal::write("\n");
    for(size_t i = 0; i < 10; i++)
    {
        vga_terminal::set_colour(VGA_COLOUR_LIGHT_RED, VGA_COLOUR_LIGHT_RED);
        vga_terminal::write(" ");
    }

}
