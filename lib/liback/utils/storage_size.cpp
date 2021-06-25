#include "liback/utils/storage_size.h"
#include <cstdlib>
#include <cstdio>
#include <functional>

#define KB_SIZE 1024L
#define MB_SIZE (KB_SIZE * KB_SIZE)
#define GB_SIZE (MB_SIZE * KB_SIZE)
#define TB_SIZE (GB_SIZE * KB_SIZE)
#define PB_SIZE (TB_SIZE * KB_SIZE)

namespace utils
{
    std::string format_storage_size(uint64_t size_in_bytes)
    {
        std::string res; /* TODO: std::string is quite buggy right now. Some code here temporary */

        // trying a strange programming style here
        const char* units[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB" };
        std::function<bool(uint64_t)> is[] =
        { 
            [](uint64_t value) { return value < KB_SIZE; }, 
            [](uint64_t value) { return value >= KB_SIZE && value < MB_SIZE; },
            [](uint64_t value) { return value >= MB_SIZE && value < GB_SIZE; },
            [](uint64_t value) { return value >= GB_SIZE && value < TB_SIZE; },
            [](uint64_t value) { return value >= TB_SIZE && value < PB_SIZE; },
            [](uint64_t value) { return value >= PB_SIZE; }
        };

        std::function<uint64_t(uint64_t)> convert[] =
        { 
            [](uint64_t value) { return value; }, 
            [](uint64_t value) { return value / KB_SIZE; },
            [](uint64_t value) { return value / MB_SIZE; },
            [](uint64_t value) { return value / GB_SIZE; },
            [](uint64_t value) { return value / TB_SIZE; },
            [](uint64_t value) { return value / PB_SIZE; }
        };

        for(int i = 0; i < 6; i++)
        {
            if(is[i](size_in_bytes))
            {
                char size_buffer[20];
                ultoa(convert[i](size_in_bytes), size_buffer, 10); /* TODO: use std::to_string() */

                char buff[50];
                sprintf(buff, "%s%s", size_buffer, units[i]); /* TODO: use string.append() */

                res = buff;

                break;
            }
        }

        return res;
    }
}