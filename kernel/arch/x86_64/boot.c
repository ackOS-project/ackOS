#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernel/lib/log.h"

#include "kernel/arch/x86_64/instr.h"
#include "kernel/arch/x86_64/cpuid.h"
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

void kputs(const char* msg)
{
    struct limine_terminal* terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, msg, strlen(msg));
}

void x86_begin(void)
{
    if(terminal_request.response == NULL || terminal_request.response->terminal_count < 1)
    {
        halt();
    }

    kprintf("Hello World, %d", 123);

    halt();
}
