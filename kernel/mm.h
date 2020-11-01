#pragma once

#include <cstddef>
// Memory Manager

void *malloc(size_t sz);

// Free a block allocated by malloc, realloc or calloc.
void free(void* mem);

void *memset(void *ptr, int value, size_t num);
#pragma once

void *memset(void *ptr, int value, size_t num);
