#pragma once

#include <cstddef>
#include <cstdint>
// Memory Manager

void* heap_allocate(size_t sz);

void heap_deallocate(void* mem);

void heap_initialise(uint64_t start, uint64_t end);
