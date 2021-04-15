/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 - 2021 Cael Rasmussen
* Under the GPL2 licence
******************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "kernel/kernel.h"
#include "kernel/multiboot2.h"
#include <cstdio>
#include <string>
#include "x86_64/features/CPUID.h"

#include "kernel/io/fdm.h"

void ksh_start();

void kmain(multiboot_header* header)
{
    legacy_vga::initialize(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    psf_initialize();
    memory_initalize(0x2000000, 0x3000000);

    legacy_vga::write("ackOS v0.01 built on " BUILD_TIME "\n");
    legacy_vga::write("Greetings, \n");

    fdm::init();

    // ksh_start();

    // double fault -
    // int i = 0 / 0;

    // GP fault -
    // asm volatile("int $3");

    serial::putc(SERIAL_COM1, '\n'); // leave a new line at the end

    return;
}
