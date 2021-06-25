#pragma once

#include "kernel/io/fs_node.h"

class directory_node : public fs_node
{
private:
    const char* _filename;

public:
    directory_node(const char* filename);

    const char* get_filename() const { return _filename; }

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;
};
