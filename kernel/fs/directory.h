#pragma once

#include "kernel/io/fs_node.h"

#include <vector>
#include <string>
#include <memory>

struct file_entry_t
{
private:
    std::string _filename;
    fs_node* _node;

public:
    file_entry_t()
    :
    _node(nullptr)
    {
    }

    file_entry_t(const std::string& filename, fs_node* node)
    :
    _filename(filename),
    _node(node)
    {
    }

    const std::string& get_filename() const;
    void set_filename(const std::string& name);

    fs_node* get_node() const;
    void set_node(fs_node* node);

    explicit operator bool() const { return _node != nullptr; }
};

class directory_node_t : public fs_node
{
private:
    std::vector<file_entry_t> _files;
    directory_node_t* _parent;

public:
    directory_node_t(directory_node_t* parent);

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;

    file_entry_t find_entry(const char* filename);
    directory_node_t* get_parent() { return _parent; }

    utils::result link(const char* filename, fs_node* node);
};
