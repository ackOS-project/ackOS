#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <stdio.h>

#include "kernel/lib/log.h"

#include "kernel/arch/x86_64/com.h"
#include "kernel/arch/x86_64/gdt.h"
#include "kernel/arch/x86_64/idt.h"
#include "kernel/arch/x86_64/cpuid.h"
#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/mem.h"

void kpanic(void)
{
    while(true) halt();
}

void x86_begin(void)
{
    init_com(COM_PORT1, 115200);

    init_gdt();
    init_idt();

/* #pragma GCC diagnostic ignored "-Wdiv-by-zero"
    int i = 0 / 0;
    (void)i; */

    init_memory();

    kprintf("Hello, ackOS World!\n");

    halt();
}
