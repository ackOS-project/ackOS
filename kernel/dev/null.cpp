#include "kernel/dev/null.h"

null_device::null_device()
:
fs_node(NODE_TYPE_DEVICE)
{
}

null_device::~null_device()
{
}

utils::result null_device::read(void* buff, size_t size, size_t* total_read) const
{
    *total_read = 0;

    return utils::result::SUCCESS;
}

utils::result null_device::write(const void* buff, size_t size, size_t* total_written)
{
    *total_written = size;

    return utils::result::SUCCESS;
}
