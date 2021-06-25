#include <fcntl.h>
#include <cerrno>
#include "kernel/io/handle.h"
#include <cstdio>

handle::handle(int fd, fs_node* node, int flags)
{
    this->_fd = fd;
    this->_node = node;
    this->_flags = flags;
}

utils::result handle::read(void* buff, size_t size, size_t* total_read)
{
    if(this->_flags & O_RDONLY || this->_flags & O_RDWR)
    {
        return this->_node->read(buff, size, total_read);
    }

    return utils::result::ERR_NOT_READABLE;
}

utils::result handle::write(const void* buff, size_t size, size_t* total_written)
{
    if(this->_flags & O_WRONLY || this->_flags & O_RDWR)
    {
        return this->_node->write(buff, size, total_written);
    }

    return utils::result::ERR_NOT_WRITABLE;
}

utils::result handle::io_call(int request, void* arg)
{
    return this->_node->io_call(request, arg);
}

int handle::get_index()
{
    return this->_fd;
}

handle::~handle()
{
    if(this->_node != nullptr)
    {
        delete this->_node;
    }
}
