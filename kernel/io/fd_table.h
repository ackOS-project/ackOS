#pragma once

#include "kernel/io/fs_node.h"

#include <cstdint>
#include <cstddef>
#include <liback/utils/result.h>

#define HANDLE_LIMIT 512

class fd_table
{
private:
    fs_node* _nodes[HANDLE_LIMIT];

public:
    fd_table();
    ~fd_table();

    utils::result read(int fd, void* buff, size_t size, size_t* total_read);
    utils::result write(int fd, const void* buff, size_t size, size_t* total_written);

    utils::result io_call(int fd, int request, void* arg);

    utils::result clone(int old_fd, int new_fd);
    utils::result clone(int* new_fd, int old_fd);

    int insert_node_at(int fd, fs_node* node, int flags);
    int append_node(fs_node* node, int flags);
    void remove_node(int fd);
};
