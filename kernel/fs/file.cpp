#include "kernel/fs/file.h"

#include <fcntl.h>

file_node::file_node(const char* filename, char* data_addr, size_t size, int flags)
:
fs_node(NODE_TYPE_REGULAR),
_filename(filename),
_data_addr(data_addr),
_size(size),
_flags(flags)
{
}

utils::result file_node::read(void* buff, size_t size, size_t* total_read) const
{
    char* str = (char*)buff;

    for(int i = 0; i < size; i++)
    {
        if(i > _size) break;

        str[i] = _data_addr[i];

        ++*total_read;
    }

    str[*total_read] = '\0';

    return utils::result::SUCCESS;
}

utils::result file_node::write(const void* buff, size_t size, size_t* total_written)
{
    const char* str = (const char*)buff;

    for(int i = 0; i < size; i++)
    {
        _data_addr[_offset + i] = str[i];

        ++*total_written;
    }

    return utils::result::SUCCESS;
}
