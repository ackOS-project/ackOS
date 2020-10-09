/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL licence
******************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__linux__)
#error "You are using a standard gcc/clang compiler. You will need a cross compiler for this to compile succesfully."
#endif

#if !defined(__x86_64__)
#error "This OS needs to be compiled with a x86_64-elf compiler."
#endif

#include "kernel.h"
#include "stdio.h"
#include "cpp-stl/vector.h"

extern "C" void kernel_main()
{
        vga_terminal::vga_terminal();
        initialize_serial_port(COM1, 9600);
        serial_write(COM1, 'O');
        serial_write(COM1, 'K');


        vga_terminal::write("Hello");
        vga_terminal::write("Booted ackOS! meow.\n");

        for(size_t i = 0; i < 10; i++)
        {
          vga_terminal::set_colour((vga_colour)i, (vga_colour)i);
          vga_terminal::write(" ");
        }
        vga_terminal::write("\n");
        for(size_t i = 0; i < 10; i++)
        {
          vga_terminal::set_colour(VGA_COLOUR_CYAN, VGA_COLOUR_LIGHT_RED);
          vga_terminal::write(" ");
        }
}
