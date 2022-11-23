#include <limine/limine.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static void hlt(void)
{
    while(true)
    {
        __asm__ volatile("hlt");
    }
}

void x86_begin(void)
{
    if(terminal_request.response == NULL || terminal_request.response->terminal_count < 1)
    {
        hlt();
    }

    struct limine_terminal* terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, "Hello ackOS World!", 19);

    hlt();
}
