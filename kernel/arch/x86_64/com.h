#pragma once

#include <liback/util.h>

enum
{
    COM_PORT1 = 0x3f8,
    COM_PORT2 = 0x2f8,
    COM_PORT3 = 0x3e8,
    COM_PORT4 = 0x2e8
};

void init_com(int port, int baud_rate);

void com_send_char(int port, char c);
char com_receive_char(int port);
void com_write(int port, const char* s, size_t len);
