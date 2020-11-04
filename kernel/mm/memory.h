#pragma once

#include <cstddef>
#include <cstdint>
// Memory Manager

void* memory_alloc(size_t sz);

void* memory_calloc(size_t sz);

void memory_free(void* mem);

void *memory_set(void *ptr, int value, size_t num);

void memory_initalize(uint64_t location, uint64_t length);
