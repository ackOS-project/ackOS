#pragma once

#include <cstdint>
#include <cstddef>

#include <liback/utils/result.h>

enum node_type
{
    NODE_TYPE_NONE,
    NODE_TYPE_REGULAR,
    NODE_TYPE_DIRECTORY,
    NODE_TYPE_SYMBOLIC_LINK,
    NODE_TYPE_PIPE,
    NODE_TYPE_SOCKET,
    NODE_TYPE_DEVICE,
};

class fs_node
{
private:
    node_type _type;

public:
    fs_node(node_type type = NODE_TYPE_NONE);
    virtual ~fs_node() { }

    node_type get_type() { return _type; }

    virtual utils::result read(void* buff, size_t size, size_t* total_read) const;
    virtual utils::result write(const void* buff, size_t size, size_t* total_written);

    virtual utils::result io_call(int request, void* args);
};
