/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL licence
******************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "stdio.h"
#include <vector>

void kmain()
{
    vga_terminal::initialize(VGA_COLOUR_LIGHT_GREY);

    serial_port_initialize(COM1, 9600);
    serial_write(COM1, "Hello world!\n"); // TODO: char arrays are not working!

    vga_terminal::write("Hello");
    vga_terminal::write("Booted ackOS! meow.\n");
    for(size_t i = 0; i < 10; i++)
    {
        vga_terminal::set_colour(VGA_COLOUR_GREEN, (vga_colour)i);
        vga_terminal::write(" ");
    }
    vga_terminal::write("\n");
    for(size_t i = 0; i < 10; i++)
    {
        vga_terminal::set_colour(VGA_COLOUR_CYAN, VGA_COLOUR_LIGHT_RED);
        vga_terminal::write(" ");
    }

    return;
}
