#pragma once

#include "kernel/io/device_node.h"

class stream : public device_node
{
private:
    
public:
    stream();
    ~stream();

    // utils::result read(void* buff, size_t size, size_t* total_read); - keyboard interrupts not implemented yet
    utils::result write(const void* buff, size_t size, size_t* total_written);
};
