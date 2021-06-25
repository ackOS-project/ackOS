#pragma once

#include <cstddef>
#include <cstdint>
#include <liback/utils/macros.h>

enum tar_file_type
{
    TAR_REGULAR = '0',
    TAR_HARD_LINK = '1',
    TAR_SYMBOLIC_LINK = '2',
    TAR_CHARACTER_DEVICE = '3',
    TAR_BLOCK_DEVICE = '4',
    TAR_DIRECTORY = '5',
    TAR_PIPE = '6'
};

struct tar_file_header
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char type_flag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char dev_major[8];
    char dev_minor[8];
    char prefix[131];
    char atime[12];
    char ctime[12];
}
ATTRIBUTE_PACKED;

size_t tar_get_file_size(tar_file_header* header);

tar_file_header* tar_read_header(tar_file_header* header, uint32_t location);
