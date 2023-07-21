#include <limine/limine.h>
#include <string.h>

#include "kernel/arch/x86_64/com.h"

// we shouldn't be using the limine terminal because it is deprecated (who knows why)
// TODO: write our own terminal implementation.
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

void kputs(const char* msg)
{
    size_t msg_len = strlen(msg);

    if(terminal_request.response != NULL && terminal_request.response->terminal_count > 0)
    {
        struct limine_terminal* terminal = terminal_request.response->terminals[0];
        terminal_request.response->write(terminal, msg, msg_len);
    }

    com_write(COM_PORT1, msg, msg_len);
}
