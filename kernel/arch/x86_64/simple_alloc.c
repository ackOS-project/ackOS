#include "kernel/arch/x86_64/simple_alloc.h"
#include "kernel/lib/log.h"

#include <lib/liback/util.h>
#include <stdint.h>

#define HEAP_SIZE 64 KiB
static uint8_t heap_data[HEAP_SIZE];
static uint8_t* heap_head = heap_data;
static const uint8_t* heap_end = heap_data + HEAP_SIZE;

void* simple_allocate(size_t size)
{
    if((heap_head + size) >= heap_end)
    {
        kprintf(KERN_PANIC "simple heap full!\n");

        return NULL;
    }

    void* addr = (void*)heap_head;

    heap_head += size;

    return addr;
}

size_t simple_alloc_bytes_left(void)
{
    return (size_t)(heap_end - heap_head);
}

