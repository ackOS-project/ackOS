#include "kernel/fs/filesystem.h"

#include <filesystem>
#include <cstdio>

namespace fs = std::filesystem;

static directory_node_t* fs_root;

filesystem_info_t::filesystem_info_t()
{
    _current = fs_root;
}

filesystem_info_t::filesystem_info_t(const filesystem_info_t& parent)
{
    _current = parent._current;
}

fs_node* filesystem_info_t::find(const char* p)
{
    fs::path path(p);
    fs_node* node = _current;

    for(fs::path::iterator it = path.begin(); it != path.end(); it++)
    {
        if(*it == ".")
        {
            continue;
        }
        else if(it == path.begin() && *it == "/")
        {
            node = fs_root;
        }
        else if(node->get_type() == NODE_TYPE_DIRECTORY)
        {
            directory_node_t* dir_node = (directory_node_t*)node;

            node = dir_node->find_entry((*it).c_str()).get_node();
        }

        if(it == --(path.end()))
        {
            return node;
        }
    }

    return nullptr;
}

utils::result_tuple<fs_node*> filesystem_info_t::open(const char* path, int flags)
{
    fs_node* node = find(path);
    int err = utils::result::ERR_NO_SUCH_FILE_OR_DIRECTORY;

    if(!node)
    {
        return utils::result_tuple<fs_node*>(err);
    }

    return utils::result_tuple<fs_node*>(node);
}

void filesystem_info_t::link(const char* p, fs_node* link_node)
{
    fs::path path = std::string(p);
    fs_node* node = _current;

    for(fs::path::iterator it = path.begin(); it != path.end(); it++)
    {
        if(*it == ".")
        {
            continue;
        }
        else if(it == path.begin() && *it == "/")
        {
            node = fs_root;
        }
        else if(node->get_type() == NODE_TYPE_DIRECTORY)
        {
            directory_node_t* dir_node = (directory_node_t*)node;

            if(it == --(path.end()))
            {
                dir_node->link((*it).c_str(), link_node);

                return;
            }
            else
            {
                file_entry_t entry = dir_node->find_entry((*it).c_str());

                if(!entry)
                {
                    directory_node_t* new_dir = new directory_node_t(dir_node);

                    dir_node->link((*it).c_str(), new_dir);

                    node = new_dir;
                }
                else
                {
                    node = entry.get_node();
                }
            }
        }
    }
}

void filesystem_initialise()
{
    fs_root = new directory_node_t(nullptr);
}
