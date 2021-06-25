/*****************************************
* ackOS kernel

* Written by Cael Rasmussen
* Copyright (c) 2020 - 2021 Cael Rasmussen
* Under the GPLv2 licence
******************************************/

#include "kernel/boot_protocols/uniheader.h"
#include "kernel/boot_protocols/modules.h"
#include "kernel/drivers/legacy_vga.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/memory.h"
#include "kernel/proc/process.h"
#include "kernel/psf.h"

#include <cstdio>
#include <list>

void kmain(uniheader& header)
{
    heap_initialise(0x2000000, 0x3000000);
    memory_initialise(&header);
    processes_initialise();
    boot_modules_intialise(&header);

    puts("ackOS v0.02 built on " BUILD_TIME ".");
    puts("Copyright (c) 2020 - 2021\n");
    puts("Greetings, ");

    // ksh_start();

    putchar('\n'); // leave a new line at the end

    while(true);
}
