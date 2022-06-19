#include <cstring>

#include "kernel/dev/zero.h"

zero_device::zero_device()
:
fs_node(NODE_TYPE_DEVICE)
{
}

zero_device::~zero_device()
{
}

utils::result zero_device::read(void* buff, size_t size, size_t* total_read) const
{
    *total_read = size;

    memset(buff, 0, size);

    return utils::result::SUCCESS;
}

utils::result zero_device::write(const void* buff, size_t size, size_t* total_written)
{
    *total_written = size;

    return utils::result::SUCCESS;
}
