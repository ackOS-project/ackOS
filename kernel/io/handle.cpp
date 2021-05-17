#include <fcntl.h>
#include <cerrno>
#include "kernel/io/handle.h"

handle::handle(int fd, int flags, device_node* node)
{
    this->_fd = fd;
    this->_flags = flags;
    this->_dev = node;
}

utils::result handle::read(void* buff, size_t size, size_t* total_read)
{
    if(this->_flags & O_RDONLY || this->_flags & O_RDWR)
    {
        return this->_dev->read(buff, size, total_read);
    }

    return utils::result::ERR_NOT_READABLE;
}

utils::result handle::write(const void* buff, size_t size, size_t* total_written)
{
    if(this->_flags & O_WRONLY || this->_flags & O_RDWR)
    {
        return this->_dev->write(buff, size, total_written);
    }

    return utils::result::ERR_NOT_WRITABLE;
}

int handle::get_index()
{
    return this->_fd;
}
