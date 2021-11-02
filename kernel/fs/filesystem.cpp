#include "kernel/fs/filesystem.h"

static root_filesystem root_fs;

root_filesystem* filesystem_get_root_fs()
{
    return &root_fs;
}

fs_node* root_filesystem::open(const char* path)
{
    return nullptr;
}

void root_filesystem::link(const char* path, fs_node* node)
{
    
}
