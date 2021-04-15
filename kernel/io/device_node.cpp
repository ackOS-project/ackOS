#include "kernel/io/device_node.h"

utils::result device_node::read(void* buff, size_t size, size_t* total_read)
{
    return utils::result::ERR_UNIMPLEMENTED;
}

utils::result device_node::write(const void* buff, size_t size, size_t* total_written)
{
    return utils::result::ERR_UNIMPLEMENTED;
}
