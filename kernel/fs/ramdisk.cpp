#include "kernel/fs/ramdisk.h"
#include "kernel/fs/tar.h"
#include "kernel/fs/file.h"
#include "kernel/fs/filesystem.h"
#include "kernel/proc/process.h"

#include <cstring>
#include <fcntl.h>

#include <filesystem>

class tar_node_t : public fs_node
{
private:
    char* _data;
    size_t _size;

public:
    tar_node_t(char* data, size_t size)
    :
    fs_node(NODE_TYPE_REGULAR),
    _data(data),
    _size(size)
    {
    }

    utils::result read(void* buff, size_t size, size_t* total_read) const override
    {
        char* str = (char*)buff;

        for(int i = 0; i < size; i++)
        {
            if(i >= _size) break;

            str[i] = _data[i];

            ++*total_read;
        }

        str[*total_read] = '\0';

        return utils::result::SUCCESS;
    }

    utils::result write(const void* buff, size_t size, size_t* total_written) override
    {
        return utils::result::ERR_READ_ONLY_FS;
    }
};

void ramdisk_mount(const char* mount_point, void* addr, ramdisk_type type)
{
    if(type == RAMDISK_TYPE_TAR)
    {
        tar_file_header* header = nullptr;

        for(int i = 0; ; i++)
        {
            header = tar_read_header((tar_file_header*)addr, i);

            if(header == nullptr) break;

            if(header->type_flag == TAR_REGULAR)
            {
                char* data = (char*)header + 512;
                size_t size = tar_get_file_size(header);

                filesystem_info_t fs;
                fs.link(header->filename, new tar_node_t(data, size));
            }
        }
    }
    else if(type == RAMDISK_TYPE_ZIP)
    {
        // currently unsupported
    }
}
