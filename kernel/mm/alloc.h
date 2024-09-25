#include <stdint.h>
#include <stddef.h>

#include "kernel/mm/mem.h"

// not available until physical and virtual memory management
// has been intitialised.
void* kmalloc(size_t size, uint32_t flags);
void kfree(void* mem);