/* fdm.h: File descriptor manager */
#pragma once

#include <liback/utils/result.h>

namespace fdm
{
    void init();

    utils::result read(int fd, void* buff, size_t size, size_t* total_read);
    utils::result write(int fd, const void* buff, size_t size, size_t* total_written);
}
