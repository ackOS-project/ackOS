#pragma once

#include "kernel/io/fs_node.h"
#include "kernel/fs/filesystem.h"

#include <cstdint>
#include <cstddef>
#include <vector>
#include <liback/utils/result.h>

class fd_table_t
{
private:
    std::vector<fs_node*> _nodes;
    filesystem_info_t _filesystem;

public:
    fd_table_t();
    ~fd_table_t();

    utils::result read(int fd, void* buff, size_t size, size_t* total_read);
    utils::result write(int fd, const void* buff, size_t size, size_t* total_written);

    utils::result io_call(int fd, int request, void* arg);

    utils::result clone(int old_fd, int new_fd);
    utils::result clone(int* new_fd, int old_fd);

    utils::result seek(int fd, int whence, off_t off);

    utils::result open(int* fd, const char* path, int flags);

    fs_node* get_node(int fd);
    int insert_node_at(int fd, fs_node* node);
    int append_node(fs_node* node);
    utils::result remove_node(int fd);

    void dump();
};
