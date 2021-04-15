#pragma once

#include <liback/utils/result.h>
#include <stdint.h>
#include <stddef.h>

class device_node
{
public:
    device_node() { }
    virtual ~device_node() { }

    virtual bool readable() { return true; };
    virtual bool writable() { return true; };

    virtual utils::result read(void* buff, size_t size, size_t* total_read);
    virtual utils::result write(const void* buff, size_t size, size_t* total_written);
};
