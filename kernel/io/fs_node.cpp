#include "kernel/io/fs_node.h"

fs_node::fs_node(node_type type)
{
    _type = type;
}

utils::result fs_node::read(void* buff, size_t size, size_t* total_read) const
{
    return utils::result::ERR_UNIMPLEMENTED;
}

utils::result fs_node::write(const void* buff, size_t size, size_t* total_written)
{
    return utils::result::ERR_UNIMPLEMENTED;
}

utils::result fs_node::io_call(int request, void* arg)
{
    return utils::result::ERR_UNIMPLEMENTED;
}
