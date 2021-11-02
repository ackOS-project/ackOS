/*****************************************
* ackOS kernel

* Copyright (c) 2020 - 2021 Cael Rasmussen
* Under the GPLv2 licence
******************************************/

#include "kernel/boot_protocols/modules.h"
#include "kernel/boot_protocols/uniheader.h"
#include "kernel/mm/memory.h"
#include "kernel/proc/process.h"
#include "kernel/psf.h"
#include "kernel/sys/sys_info.h"

#include <cstdio>

void print_greeting()
{
    ackos::system_info os_info = get_system_info();
    printf("Welcome to %s %s built on %s.\n%s\n", os_info.os_name.c_str(), os_info.os_release.c_str(), os_info.build_date.c_str(), os_info.copyright.c_str());
}

void kmain(uniheader& header)
{
    processes_initialise();
    memory_initialise(&header);
    boot_modules_intialise(&header);

    print_greeting();
    memory_dump();

    putchar('\n'); // leave a new line at the end

    while(true);
}
