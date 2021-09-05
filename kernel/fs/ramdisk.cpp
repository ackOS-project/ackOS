#include "kernel/fs/ramdisk.h"
#include "kernel/fs/tar.h"
#include "kernel/fs/file.h"
#include "kernel/proc/process.h"

#include <cstring>
#include <cstdio>
#include <fcntl.h>

void ramdisk_mount(const char* mount_point, void* addr, ramdisk_type type)
{
    if(type == RAMDISK_TYPE_TAR)
    {
        tar_file_header* header = nullptr;

        for(int i = 0; ; i++)
        {
            header = tar_read_header((tar_file_header*)addr, i);

            if(header == nullptr) break;

            //printf("%s\n", header->filename);

            if(header->type_flag == TAR_REGULAR)
            {
                char* data = (char*)header + 512;
                size_t size = tar_get_file_size(header);

                //file_node* node = new file_node(header->filename, data, size, 0);
                //process_get_from_pid(0)->get_fd_table().append_node(node, O_RDWR);
            }
        }
    }
    else if(type == RAMDISK_TYPE_ZIP)
    {
        // currently unsupported
    }
}
