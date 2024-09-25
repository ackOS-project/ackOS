#pragma once

#include <stddef.h>

// fast static unfreeable heap
// also don't worry about zeroing the
// memory returned since its guaranteed to
// be cleared
void* simple_allocate(size_t size);

size_t simple_alloc_bytes_left(void);
