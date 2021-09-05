#pragma once

#include "kernel/io/fs_node.h"

#include <list>
#include <string>
#include <memory>

struct file_entry
{
private:
    std::string _filename = nullptr;
    fs_node* _node = nullptr;

public:
    file_entry() { }
    file_entry(const std::string& filename, fs_node* node)
    {
        set_filename(filename);
        set_node(node);
    }

    const std::string& get_filename() const;
    void set_filename(const std::string& name);

    fs_node* get_node() const;
    void set_node(fs_node* node);

    explicit operator bool() const { return _node != nullptr; }

    ~file_entry()
    {
        if(_node != nullptr) delete _node;
    }
};

class directory_node : public fs_node
{
private:
    std::list<file_entry> _files;

public:
    directory_node();

    utils::result read(void* buff, size_t size, size_t* total_read) const override;
    utils::result write(const void* buff, size_t size, size_t* total_written) override;

    file_entry find_entry(const char* filename);

    utils::result link(const char* filename, fs_node* node);
};
