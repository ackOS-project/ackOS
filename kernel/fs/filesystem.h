#pragma once

#include "kernel/io/fs_node.h"
#include "kernel/fs/directory.h"

#include <liback/utils/result.h>

class filesystem_info_t
{
private:
    directory_node_t* _current;

public:
    filesystem_info_t();
    filesystem_info_t(const filesystem_info_t& parent);

    directory_node_t* get_current() { return _current; }
    void set_current(directory_node_t* node) { _current = node; }

    fs_node* find(const char* p);

    utils::result_tuple<fs_node*> open(const char* path, int flags);
    void link(const char* p, fs_node* link_node);

    ~filesystem_info_t()
    {
    }
};

void filesystem_initialise();
