#include "arch/x86_64/features/serial.h"
#include "kernel/io/stream.h"

stream::stream()
{
}

stream::~stream()
{
}

utils::result stream::write(const void* buff, size_t size, size_t* total_written)
{
    const char* str = (const char*)buff;

    for(int i = 0; i < size; i++)
    {
        serial::putc(SERIAL_COM1, str[i]);

        ++*total_written;
    }

    return utils::result::SUCCESS;
}
