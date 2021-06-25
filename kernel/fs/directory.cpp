#include "kernel/fs/directory.h"

#include <fcntl.h>

directory_node::directory_node(const char* filename)
:
_filename(filename)
{
}

utils::result directory_node::read(void* buff, size_t size, size_t* total_read) const
{
    return utils::result::ERR_FILE_IS_A_DIRECTORY;
}

utils::result directory_node::write(const void* buff, size_t size, size_t* total_written)
{
    return utils::result::ERR_FILE_IS_A_DIRECTORY;
}
