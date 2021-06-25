#pragma once

#include "kernel/io/fs_node.h"

class file_node : public fs_node
{
private:
    const char* _filename;
    char* _data_addr;
    int _flags = 0;

    int _offset = 0;
    size_t _size = 0;

public:
    file_node(const char* filename, char* data_addr, size_t size, int flags);

    const char* get_filename() const { return _filename; }

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;
};
