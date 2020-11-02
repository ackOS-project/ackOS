/*
TODO: re-write memory manager
*/
#include "kernel/mm.h"
#include <cstring>

struct heap_block
{
    uint64_t size;

    heap_block* next;
    heap_block* next_free;
    heap_block* previous;
    heap_block* previous_free;

    bool is_free;
};

static heap_block* heap_start;

void merge_blocks(heap_block* a, heap_block* b)
{
    if(a == nullptr) return;
    if(b == nullptr) return;

    if(a < b)
    {
        a->size += b->size + sizeof(heap_block);
        a->next = b->next;
        a->next_free = b->next_free;

        b->next->previous = a;
        b->next->previous_free = a;
        b->next_free->previous_free = a;
    }
    else
    {
        b->size += a->size + sizeof(heap_block);
        b->next = a->next;
        b->next_free = a->next_free;

        a->next->previous = b;
        a->next->previous_free = b;
        a->next_free->previous_free = b;
    }
}

void free(void* mem)
{
    heap_block* block = ((heap_block*)mem) - 1;
    block->is_free = true;

    if(block < heap_start)
    {
        heap_start = block;
    }

    if(block->next_free != nullptr)
    {
        if(block->next_free->previous_free < block)
        {
            block->next_free->previous_free = block;
        }
    }

    if(block->next != nullptr)
    {
        block->next->previous = block;
        if(block->next->is_free) merge_blocks(block, block->next);
    }

    if(block->previous_free != nullptr)
    {
        if(block->previous_free->next_free > block)
        {
            block->previous_free->next_free = block;
        }
    }

    if(block->previous != nullptr)
    {
        block->previous->next = block;
        if(block->previous->is_free) merge_blocks(block, block->previous);
    }
}

void *memset(void *ptr, int value, size_t num)
{
	unsigned char *p = (unsigned char *)ptr;
	while (num--)
    {
        *p++ = (unsigned char)value;
    }
	return ptr;
}

void memory_initalize(uint64_t location, uint64_t size)
{
    heap_start = (heap_block*)location;
    heap_start->size = size - sizeof(heap_block);

    heap_start->next = nullptr;
    heap_start->next_free = nullptr;
    heap_start->previous = nullptr;
    heap_start->previous_free = nullptr;

    heap_start->is_free = true;
}

void* malloc(size_t size)
{
    size -= (size % 8);
    if((size % 8) != 0) size += 8;

    heap_block* block = heap_start;

    while (true)
    {
        if(block->size >= size)
        {
            if(block->size > size + sizeof(heap_block))
            {
                heap_block* new_block = (heap_block*)block + sizeof(heap_block) + size;

                new_block->is_free = true;
                new_block->size = block->size - sizeof(heap_block) + size;

                new_block->next_free = block->next_free;
                new_block->next = block->next;
                new_block->previous_free = block->previous_free;
                new_block->previous = block;

                block->next_free = new_block;
                block->next = new_block;
                block->size = size;
            }
            if(block == heap_start)
            {
                heap_start = block->next_free;
            }
            block->is_free = false;

            if(block->previous_free != nullptr)
            {
                block->previous_free->next_free = block->next_free;
            }

            if(block->previous != nullptr)
            {
                block->previous->next_free = block->next_free;
            }

            if(block->next_free != nullptr)
            {
                block->next_free->previous_free = block->previous_free;
            }

            if(block->next != nullptr)
            {
                block->next->previous_free = block->previous_free;
            }

            return block + 1;
        }

        if(block->next_free == nullptr)
        {
            // Out of memory
            return nullptr;
        }

        block = block->next_free;
    }
    return nullptr;
}

void* calloc(size_t size)
{
    void* mem = malloc(size);
    memset(mem, 0, size);

    return mem;
}
