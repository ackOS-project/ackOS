#include "kernel/fs/tar.h"

size_t tar_get_file_size(tar_file_header* header)
{
    size_t size = 0;
    size_t count = 1;
 
    for (size_t j = 11; j > 0; j--, count *= 8)
    {
        size += ((header->size[j - 1] - TAR_REGULAR) * count);
    }

    return size;
}

tar_file_header* tar_read_header(tar_file_header* header, uint32_t location)
{
    for(uint32_t i = 0; i < location; i++)
    {
        if(header->filename[0] == '\0')
        {
            return nullptr;
        }

        uint32_t size = tar_get_file_size(header);

        header = (tar_file_header*)((char*)header + ((size / 512) + 1) * 512);

        if(size % 512)
        {
            header = (tar_file_header*)((char*)header + 512);
        }
    }

    return header;
}
