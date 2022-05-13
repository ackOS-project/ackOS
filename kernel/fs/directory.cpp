#include "kernel/fs/directory.h"
#include "kernel/arch/arch.h"

#include <fcntl.h>
#include <cstring>

const std::string& file_entry_t::get_filename() const
{
    return _filename;
}

void file_entry_t::set_filename(const std::string& name)
{
    _filename = name;
}

fs_node* file_entry_t::get_node() const
{
    return _node;
}

void file_entry_t::set_node(fs_node* node)
{
    _node = node;
}

directory_node_t::directory_node_t(directory_node_t* parent)
:
fs_node(NODE_TYPE_DIRECTORY),
_parent(parent)
{
}

utils::result directory_node_t::read(void* buff, size_t size, size_t* total_read) const
{
    return utils::result::ERR_FILE_IS_A_DIRECTORY;
}

utils::result directory_node_t::write(const void* buff, size_t size, size_t* total_written)
{
    return utils::result::ERR_FILE_IS_A_DIRECTORY;
}

file_entry_t directory_node_t::find_entry(const char* filename)
{
    for(auto& file : _files)
    {
        if(file.get_filename() == filename)
        {
            return file;
        }
    }

    return {};
}

utils::result directory_node_t::link(const char* filename, fs_node* node)
{
    if(find_entry(filename))
    {
        return utils::result::ERR_FILE_EXISTS;
    }

    _files.push_back(file_entry_t(filename, node));

    return utils::result::SUCCESS;
}
