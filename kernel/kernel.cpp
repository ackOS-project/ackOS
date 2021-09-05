/*****************************************
* ackOS kernel

* Copyright (c) 2020 - 2021 Cael Rasmussen
* Under the GPLv2 licence
******************************************/

#include "kernel/boot_protocols/uniheader.h"
#include "kernel/boot_protocols/modules.h"
#include "kernel/mm/heap.h"
#include "kernel/mm/memory.h"
#include "kernel/proc/process.h"
#include "kernel/sys_info.h"
#include "kernel/psf.h"

#include <cstdio>

void ksh_start(FILE* out_stream, FILE* in_stream);
void __stdlib_init();

void kmain(uniheader& header)
{
    __stdlib_init();

    heap_initialise(0x5000000, 0x6000000);
    memory_initialise(&header);
    processes_initialise();
    boot_modules_intialise(&header);

    ackos::system_info os_info = get_system_info();
    printf("Welcome to %s %s built on %s.\n%s\n\n", os_info.os_name.c_str(), os_info.os_release.c_str(), os_info.build_date.c_str(), os_info.copyright.c_str());

    putchar('\n'); // leave a new line at the end

    while(true);
}
