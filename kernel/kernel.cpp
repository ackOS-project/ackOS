/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 Cael Rasmussen
* Under the GPL2 licence
******************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "kernel/kernel.h"
#include "kernel/multiboot2.h"
#include <cstdio>
#include <string>

void ksh_start();

void kmain()
{
    legacy_vga::initialize(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    psf_initialize();
    serial::port_initialize(COM1, 9600);
    memory_initalize(0x1000000, 0x1000000);

    legacy_vga::write("Hello\n");
    legacy_vga::write("Booted ackOS! meow.\n");

    ksh_start();
    
    serial::putc(COM1, '\n'); // leave a new line at the end
}
