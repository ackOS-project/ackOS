#pragma once

#include "kernel/io/fs_node.h"

class early_console_device : public fs_node
{
public:
    early_console_device();
    ~early_console_device();

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;

    utils::result set_offset(int whence, off_t offset) override;
};
