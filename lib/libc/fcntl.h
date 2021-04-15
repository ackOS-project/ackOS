#pragma once

#include <cstdarg>
#include <cstddef>

enum
{
    O_RDONLY = 0x00,
    O_WRONLY = 0x1,
    O_RDWR = 0x2,
    O_CREAT = 0x100,
    O_TRUNC = 0x1000,
    O_APPEND = 0x2000,
    O_DIRECTORY = 0x200000
};
