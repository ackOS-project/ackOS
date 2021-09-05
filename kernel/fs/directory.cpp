#include "kernel/fs/directory.h"

#include <fcntl.h>
#include <cstring>

const std::string& file_entry::get_filename() const
{
    return _filename;
}

void file_entry::set_filename(const std::string& name)
{
    _filename = name;
}

fs_node* file_entry::get_node() const
{
    return _node;
}

void file_entry::set_node(fs_node* node)
{
    _node = node;
}

directory_node::directory_node()
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

file_entry directory_node::find_entry(const char* filename)
{
    file_entry res;

    for(file_entry& file : _files)
    {
        if(file.get_filename() == filename)
        {
            res = file;

            break;
        }
    }

    return res;
}

utils::result directory_node::link(const char* filename, fs_node* node)
{
    if(find_entry(filename))
    {
        return utils::result::ERR_FILE_EXISTS;
    }

    _files.push_back({ filename, node });

    return utils::result::SUCCESS;
}
