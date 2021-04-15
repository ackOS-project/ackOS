#pragma once

#define SERIAL_COM1 0x3f8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

namespace serial
{
    void port_initialize(int port, int baud_rate);

    int transmit_empty(int port);

    void putc(int port, char a);

    char getc(int port);

    void print(int port, const char *data, int size = -1);

    void print_int(int port, int i);
}
