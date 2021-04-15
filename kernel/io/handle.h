#pragma once

#include <liback/utils/result.h>
#include <stdint.h>
#include <stddef.h>
#include "kernel/io/device_node.h"

class handle
{
private:
    int _fd;
    int _flags;
    device_node* _dev;

public:
    handle(int fd, int flags, device_node* node);

    utils::result read(void* buff, size_t size, size_t* total_read);
    utils::result write(const void* buff, size_t size, size_t* total_written);

    int get_index();
};

void handles_init();
