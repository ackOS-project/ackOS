#pragma once

#include <cstddef>
#include <cstdint>
// Memory Manager

void* memory_alloc(size_t sz);

void memory_free(void* mem);

void memory_initalize(uint64_t start, uint64_t end);
