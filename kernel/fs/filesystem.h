#pragma once

#include "kernel/io/fs_node.h"
#include "kernel/fs/directory.h"

class root_filesystem
{
private:
    directory_node* _root;

public:
    root_filesystem()
    {
    }

    root_filesystem(const root_filesystem& other) = delete;

    directory_node* get_root() { return _root; }
    void set_root(directory_node* root) { _root = root; }

    fs_node* open(const char* path);
    void link(const char* path, fs_node* node);

    ~root_filesystem()
    {
        if(_root != nullptr) delete _root;
    }
};

root_filesystem* filesystem_get_root_fs();
