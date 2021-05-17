/*****************************************
* Written by Cael Rasmussen
* Copyright (c) 2020 - 2021 Cael Rasmussen
* Under the GPL2 licence
******************************************/
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <cstdio>

#include "kernel/boot_protocols/uniheader.h"
#include "kernel/io/fdm.h"
#include "kernel/drivers/legacy_vga.h"
#include "kernel/mm/heap.h"
#include "kernel/psf.h"

void ksh_start();

void kmain(void* header, uint32_t magic)
{
    legacy_vga::initialize(VGA_COLOUR_WHITE, VGA_COLOUR_BLACK);
    psf_initialise();
    heap_initialise(0x2000000, 0x3000000);

    legacy_vga::write("ackOS v0.02 built on " BUILD_TIME "\n");
    legacy_vga::write("Greetings, \n");

    fdm::init();

    uniheader uheader;
    uniheader_parse(uheader, header, magic);

    printf("bootloader_name: %s\ncommand_line: %s\nframebuffer: addr=0x%x; width=%d; height=%d\n", uheader.bootloader_name, uheader.command_line_args, uheader.framebuffer_addr, uheader.framebuffer_width, uheader.framebuffer_height);

    uint32_t *fb_addr = (uint32_t*)uheader.framebuffer_addr;

    for (size_t i = 0; i < uheader.framebuffer_width; i++)
    {
        fb_addr[i] = 0x34eb7d;
        fb_addr[i + uheader.framebuffer_width] = 0x34eb7d;
        fb_addr[i + (uheader.framebuffer_width * 2)] = 0x34eb7d;
        fb_addr[i + (uheader.framebuffer_width * 3)] = 0x34eb7d;
        fb_addr[i + (uheader.framebuffer_width * 4)] = 0x34eb7d;
        for(int i = 0; i < 1000000; i++);
    }

    // ksh_start();

    putchar('\n'); // leave a new line at the end

    while(true);
}
