#pragma once

#include "kernel/io/fs_node.h"

#define KERNEL_DEVICE_ZERO "/dev/zero"

class zero_device : public fs_node
{
private:

public:
    zero_device();
    ~zero_device();

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;
};
