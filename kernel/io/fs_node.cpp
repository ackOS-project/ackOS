#include <unistd.h>

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

utils::result fs_node::set_offset(int whence, off_t off)
{    
    return utils::result::ERR_UNIMPLEMENTED;
}

#include "kernel/io/node_types_macro.h"
#define NODE_TYPE_GENERATE_STRINGS(__name) #__name,
static const char* _node_strings[] =
{
    NODE_TYPE_MACRO(NODE_TYPE_GENERATE_STRINGS)
};
#undef NODE_TYPE_GENERATE_STRINGS
#undef NODE_TYPE_MACRO

const char* fs_node_type_to_string(node_type type)
{
    return _node_strings[type];
}
