#include "arch/x86_64/features/com.h"
#include "kernel/dev/com.h"

static int com_port_to_address(int port)
{
    switch (port)
    {
    case 1:
    {
        return x86_64::COM1;

        break;
    }
    case 2:
    {
        return x86_64::COM2;

        break;
    }
    case 3:
    {
        return x86_64::COM3;

        break;
    }
    case 4:
    {
        return x86_64::COM4;

        break;
    }

    default:
        break;
    }

    return 0;
}

com_port::com_port(int port)
:
fs_node(NODE_TYPE_DEVICE)
{
    _addr = com_port_to_address(port);
}

com_port::~com_port()
{
}

utils::result com_port::read(void* buff, size_t size, size_t* total_read) const
{
    char* str = (char*)buff;

    for(int i = 0; i < size; i++)
    {
        str[i] = x86_64::com_getc(_addr);

        ++*total_read;
    }

    return utils::result::SUCCESS;
}

utils::result com_port::write(const void* buff, size_t size, size_t* total_written)
{
    const char* str = (const char*)buff;

    for(int i = 0; i < size; i++)
    {
        x86_64::com_putc(_addr, str[i]);

        ++*total_written;
    }

    return utils::result::SUCCESS;
}
