#include "kernel/io/fd_table.h"
#include "kernel/sys/logger.h"

#include <fcntl.h>
#include <liback/utils/result.h>

fs_node* fd_table_t::get_node(int fd)
{
    if(fd > 0 && fd < _nodes.size())
    {
        return _nodes[fd];
    }

    return nullptr;
}

int fd_table_t::insert_node_at(int fd, fs_node* node)
{
    if(fd >= _nodes.size())
    {
        size_t cur_size = _nodes.size();

        _nodes.resize(fd + 1);

        for(size_t i = cur_size; i < fd; i++)
        {
            _nodes[i] = nullptr;
        }
    }

    _nodes[fd] = node;

    return fd;
}

int fd_table_t::append_node(fs_node* node)
{
    int fd = -1;

    for(int i = 0; i < _nodes.size(); i++)
    {
        if(_nodes[i] == nullptr)
        {
            fd = i;
            _nodes[i] = node;
            break;
        }
    }

    if(fd == -1)
    {
        fd = _nodes.size();
        _nodes.push_back(node);
    }

    return fd; 
}

utils::result fd_table_t::remove_node(int fd)
{
    utils::result result = utils::result::ERR_INVALID_FD;
        
    if(fd > 0 && fd < _nodes.size())
    {
        if(_nodes[fd] != nullptr)
        {
            _nodes[fd] = nullptr;
            result = utils::result::SUCCESS;
        }
    }

    return result;
}

utils::result fd_table_t::read(int fd, void* buff, size_t size, size_t* total_read)
{
    fs_node* node = get_node(fd);

    *total_read = 0; /* make sure it's set to zero */

    if(node == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return node->read(buff, size, total_read);
}

utils::result fd_table_t::write(int fd, const void* buff, size_t size, size_t* total_written)
{
    fs_node* node = get_node(fd);

    *total_written = 0;

    if(node == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return node->write(buff, size, total_written);
}

utils::result fd_table_t::io_call(int fd, int request, void* arg)
{
    fs_node* node = get_node(fd);

    if(node == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return node->io_call(request, arg);
}

utils::result fd_table_t::clone(int old_fd, int new_fd)
{
    if(old_fd == new_fd)
    {
        return utils::result::ERR_INVALID_ARGUMENT;
    }

    fs_node* old_node = get_node(old_fd), *new_node = get_node(new_fd);

    if(old_node == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    if(new_node != nullptr)
    {
        remove_node(new_fd);
    }

    insert_node_at(new_fd, old_node);

    return utils::result::SUCCESS;
}

utils::result fd_table_t::clone(int* new_fd, int old_fd)
{
    *new_fd = -1;

    if(new_fd == nullptr)
    {
        return utils::result::ERR_INVALID_ADDRESS;
    }
    else if(get_node(old_fd) == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    *new_fd = append_node(get_node(old_fd));

    return utils::result::SUCCESS;
}

utils::result fd_table_t::open(int* fd, const char* path, int flags)
{
    auto result = _filesystem.open(path, flags);

    if(result)
    {
        *fd = append_node(result.get_value());
    }

    return result.get_result();
}

fd_table_t::fd_table_t()
{
}

fd_table_t::~fd_table_t()
{
}

void fd_table_t::dump()
{
    log_info("fd", "Open file descriptors: ");

    for(int i = 0; i < _nodes.size(); i++)
    {
        if(_nodes[i] != nullptr)
        {
            log_info("fd", "  %d -> %s", i, fs_node_type_to_string(_nodes[i]->get_type()));
        }
    }
}

utils::result fd_table_t::seek(int fd, int whence, off_t off)
{
    if(_nodes[fd] == nullptr)
    {
        return utils::result::ERR_INVALID_FD;
    }

    return _nodes[fd]->set_offset(whence, off);
}
