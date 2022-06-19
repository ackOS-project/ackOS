/*****************************************
* ackOS kernel

* Copyright (c) 2020 - 2022 Cael Rasmussen
* Under the GPLv2 licence
******************************************/

#include "kernel/boot/modules.h"
#include "kernel/boot/uniheader.h"
#include "kernel/mm/memory.h"
#include "kernel/proc/process.h"
#include "kernel/psf.h"
#include "kernel/sys/sys_info.h"
#include "kernel/sys/logger.h"
#include "kernel/fs/filesystem.h"
#include "kernel/dev/devices.h"
#include "kernel/arch/arch.h"

#include <cstdio>

void print_greeting()
{
    ackos::system_info os_info = get_system_info();
    log_info("kernel", "Welcome to %s %s built on %s", os_info.os_name.c_str(), os_info.os_release.c_str(), os_info.build_date.c_str());
    log_info("kernel", os_info.copyright.c_str());

    const char* filename = "/ackos.txt";
    FILE* file = fopen(filename, "rw");

    printf("$ cat %s\n", filename);

    if(file == nullptr)
    {
        printf("%s: failed to open file - errno %d\n", filename, errno);
    }
    else
    {
        size_t size = 256;
        size_t count = 1;
        char buff[size * count + 1];

        size_t read = fread(buff, size, count, file);

        buff[read] = '\0';

        if(errno == 0)
        {
            printf("%s\n", buff);
        }
        else
        {
            printf("%s: could not read file - errno %d\n", filename, errno);
        }
    }
}

void kmain(uniheader& header)
{
    filesystem_initialise();
    processes_initialise();
    memory_initialise(&header);
    boot_modules_initialise(&header);
    devices_initialise(&header);

    psf_write_to_fb("Hello, World! Welcome to ackOS!");

    memory_dump();
    print_greeting();

    putchar('\n'); /* write a newline if hasn't been printed already */

    while(true)
    {
        arch::halt();
    }
}
