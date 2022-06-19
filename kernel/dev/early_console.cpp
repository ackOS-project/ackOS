#include "kernel/dev/early_console.h"
#include "kernel/arch/arch.h"

early_console_device::early_console_device()
:
fs_node(NODE_TYPE_DEVICE)
{
}

early_console_device::~early_console_device()
{
}

utils::result early_console_device::read(void* buff, size_t size, size_t* total_read) const
{
    char* str = (char*)buff;

    for(int i = 0; i < size; i++)
    {
        str[i] = arch::early_getchar();

        ++*total_read;
    }

    return utils::result::SUCCESS;
}

utils::result early_console_device::write(const void* buff, size_t size, size_t* total_written)
{
    const char* str = (const char*)buff;

    *total_written = arch::early_write(str, size);

    return utils::result::SUCCESS;
}

utils::result early_console_device::set_offset(int whence, off_t offset)
{
    return utils::result::ERR_IS_TERMINAL;
}