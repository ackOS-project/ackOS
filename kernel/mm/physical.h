#pragma once

#include <cstdint>
#include <cstddef>

#include "kernel/boot/uniheader.h"

void physical_initialise(uniheader* uheader);

bool physical_page_is_free(void* addr);

void physical_lock(void* page, size_t size);

void* physical_allocate(size_t size);

void* physical_allocate_clear(size_t size);

void physical_deallocate(void* addr, size_t size);
