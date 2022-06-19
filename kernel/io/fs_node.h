#pragma once

#include <cstdint>
#include <cstddef>

#include <sys/types.h>

#include <liback/utils/result.h>

#include "kernel/io/node_types_macro.h"
#define NODE_TYPE_GENERATE_ENUM(__name) __name,
enum node_type
{
    NODE_TYPE_MACRO(NODE_TYPE_GENERATE_ENUM)
};
#undef NODE_TYPE_GENERATE_ENUM
#undef NODE_TYPE_MACRO

class fs_node
{
private:
    node_type _type;

protected:
    int _offset = 0;

public:
    fs_node(node_type type = NODE_TYPE_NONE);
    virtual ~fs_node() { }

    node_type get_type() const { return _type; }

    off_t get_offset() const { return _offset; }
    virtual utils::result set_offset(int whence, off_t off);

    virtual utils::result read(void* buff, size_t size, size_t* total_read) const;
    virtual utils::result write(const void* buff, size_t size, size_t* total_written);

    virtual utils::result io_call(int request, void* args);
};

const char* fs_node_type_to_string(node_type type);
