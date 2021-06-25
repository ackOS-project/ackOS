#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace utils
{
    /* formats size_in_bytes using the most appropiate unit. eg. input: 4096; output: "4KiB" */
    std::string format_storage_size(uint64_t size_in_bytes);
}
