#pragma once

#define COM1 0x3f8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

namespace serial
{
   void port_initialize(int port, int baud_rate);

   int transmit_empty(int port);

   void putc(int port, char a);

   void write(int port, const char *data, int size = -1);
}
