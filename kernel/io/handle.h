#pragma once

#include <liback/utils/result.h>
#include <stdint.h>
#include <stddef.h>
#include "kernel/io/fs_node.h"

class handle
{
private:
    int _fd = 0;
    int _flags = 0;
    fs_node* _node = nullptr;

public:
    handle(int fd, fs_node* node, int flags);
    ~handle();

    utils::result read(void* buff, size_t size, size_t* total_read);
    utils::result write(const void* buff, size_t size, size_t* total_written);

    utils::result io_call(int request, void* arg);

    int get_index();
};

void handles_init();
