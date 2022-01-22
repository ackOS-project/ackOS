#include "kernel/io/fd_table.h"
#include "kernel/sys/logger.h"

#include <fcntl.h>
#include <liback/utils/result.h>

int fd_table::insert_node_at(int fd, fs_node* node, int flags)
{
    _nodes[fd] = node;

    return fd;
}

int fd_table::append_node(fs_node* node, int flags)
{
    int fd = -1;

    for(int i = 0; i < HANDLE_LIMIT; i++)
    {
        if(_nodes[i] == nullptr)
        {
            fd = insert_node_at(i, node, flags);

            break;
        }
    }

    return fd; 
}

utils::result fd_table::remove_node(int fd)
{
    utils::result result;

    if(_nodes[fd] != nullptr)
    {
        delete _nodes[fd];
        _nodes[fd] = NULL;
    }
    else
    {
        result = utils::result::ERR_INVALID_FD;
    }

    return result;
}

utils::result fd_table::read(int fd, void* buff, size_t size, size_t* total_read)
{
    *total_read = 0; /* make sure it's set to zero */

    if(fd > HANDLE_LIMIT || fd < 0)
    {
        return utils::result::ERR_OUT_OF_BOUNDS;
    }
    else if(_nodes[fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return _nodes[fd]->read(buff, size, total_read);
}

utils::result fd_table::write(int fd, const void* buff, size_t size, size_t* total_written)
{
    *total_written = 0;

    if(fd > HANDLE_LIMIT || fd < 0)
    {
        return utils::result::ERR_OUT_OF_BOUNDS;
    }
    else if(_nodes[fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return _nodes[fd]->write(buff, size, total_written);
}

utils::result fd_table::io_call(int fd, int request, void* arg)
{
    if(fd > HANDLE_LIMIT || fd < 0)
    {
        return utils::result::ERR_OUT_OF_BOUNDS;
    }
    else if(_nodes[fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return _nodes[fd]->io_call(request, arg);
}

utils::result fd_table::clone(int old_fd, int new_fd)
{
    if(old_fd == new_fd)
    {
        return utils::result::ERR_INVALID_ARGUMENT;
    }

    if((old_fd > HANDLE_LIMIT || old_fd < 0) || (new_fd > HANDLE_LIMIT || new_fd < 0))
    {
        return utils::result::ERR_OUT_OF_BOUNDS;
    }
    else if(_nodes[old_fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    if(_nodes[new_fd] != nullptr)
    {
        remove_node(new_fd);
    }

    _nodes[new_fd] = _nodes[old_fd];

    return utils::result::SUCCESS;
}

utils::result fd_table::clone(int* new_fd, int old_fd)
{
    if(new_fd == nullptr)
    {
        return utils::result::ERR_INVALID_ADDRESS;
    }

    if(_nodes[old_fd] == nullptr)
    {
        *new_fd = -1;

        return utils::result::ERR_INVALID_FD;
    }

    for(int i = 0; i < HANDLE_LIMIT; i++)
    {
        if(_nodes[i] == nullptr)
        {
            utils::result result = clone(old_fd, i);

            if(result)
            {
                *new_fd = i;
            }
            else
            {
                *new_fd = -1;
            }

            return result;
        }
    }

    return utils::result::SUCCESS;
}

utils::result fd_table::open(int* fd, const char* path, int flags)
{
    auto result = _filesystem.open(path, flags);

    if(result)
    {
        *fd = append_node(result.get_value(), flags);
    }

    return result.get_result();
}

fd_table::fd_table()
{
    for(int i = 0; i < HANDLE_LIMIT; i++)
    {
        _nodes[i] = 0;
    }
}

fd_table::~fd_table()
{
    for(int i = 0; i < HANDLE_LIMIT; i++)
    {
        if(_nodes[i] != nullptr) delete _nodes[i];
    }
}

void fd_table::dump()
{
    log_info("fd", "Open file descriptors: ");

    for(int i = 0; i < HANDLE_LIMIT; i++)
    {
        if(_nodes[i] != nullptr)
        {
            log_info("fd", "  %d -> %s", i, fs_node_type_to_string(_nodes[i]->get_type()));
        }
    }
}

utils::result fd_table::seek(int fd, off_t off)
{
    if(_nodes[fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return _nodes[fd]->set_offset(off);
}
