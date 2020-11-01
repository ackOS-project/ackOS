#pragma once

#include <cstddef>
#include <cstdint>
// Memory Manager

void *malloc(size_t sz);

// Free a block allocated by malloc, realloc or calloc.
void free(void* mem);

void *memset(void *ptr, int value, size_t num);

void memory_initalize(uint64_t location, uint64_t length);
