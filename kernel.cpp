/*****************************************
* Written by Cael Rasmussen 
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL licence
******************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
//#include "liballoc.h"
//#include "multiboot.h"

//#include <stdio.h>
//#include "conversion.h"

int sqrt(int x)
{
    int quotient = 0;
    int i = 0;

    bool resultfound = false;
    while (resultfound == false) {
        if (i*i == x) {
          quotient = i;
          resultfound = true;
        }
        i++;
    }
    return quotient;
}

#if defined(__linux__)
#error "You are using a standard gcc/clang compiler. You will need a cross compiler for this to compile succesfully."
#endif
 
#if !defined(__i386__)
#error "This OS needs to be compiled with a ix86-elf compiler."
#endif

#include "kernel.h"
#include "stdio.h"
#include "cpp-stl/vector.h"

extern "C" void kernel_main(void) 
{
        vga_terminal::vga_terminal(VGA_COLOUR_BLACK, VGA_COLOUR_WHITE);

        vga_terminal::write("Booted ackOS! meow\n");
        //printf("value: %s and the int is %i and %i","good",666, 777);
        //vga_terminal::write("\nhe s\n");
        for(size_t i = 0; i < 10; i++)
        {
          vga_terminal::set_colour(VGA_COLOUR_RED,VGA_COLOUR_RED);
          vga_terminal::write(" ");
          //vga_terminal::write();
        }
        vga_terminal::write("\n");
        for(size_t i = 0; i < 10; i++)
        {
          vga_terminal::set_colour(VGA_COLOUR_LIGHT_RED,VGA_COLOUR_LIGHT_RED );
          vga_terminal::write(" ");
          //vga_terminal::write();
        }

        //*i = 9;
        //printf("value=%i", ptr[1]);

}
