#pragma once

#include "kernel/io/fs_node.h"

class com_port : public fs_node
{
private:
    int _addr;

public:
    com_port(int port);
    ~com_port();

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;
};
