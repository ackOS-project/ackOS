/*
TODO: re-write memory manager
*/
#include "kernel/mm.h"
#include <cstring>

typedef struct heap_block
{
    struct heap_block* next;
    size_t size;
    bool is_free;
    int x; // for alligment
} header;

void set_data_to_block(header *block, size_t sz);
header *to_end_data(header *block);

//#define Heap_Capacity 2000000 // Lots
#define Heap_Capacity 10000     // Not much
unsigned long long active_size; // bytes in active allocations

static uint64_t heap[Heap_Capacity / sizeof(uint64_t)];

static char *max_heap = (char *)(&heap[0]);
static char *min_heap = (char *)(&heap[0]);
static const char* END_CHK = "\xDE\xAD\xC0\xDA";
header *last_allocated;

void *malloc(size_t sz)
{
    if (sz == 0 || sz > Heap_Capacity)
    {
        return NULL;
    }

    header *block = (header *)heap;
    if (active_size == 0)
    {
        set_data_to_block(block, sz);
        return (void *)last_allocated;
    }

    while (block->next != NULL)
    {
        block = block->next;
    }

    block->next = (header *)((char *)to_end_data(block) + 8);
    header *new_block = block->next;
    set_data_to_block(new_block, sz);

    return (void *)last_allocated;
}

// Free a block allocated by malloc, realloc or calloc.
void free(void* mem)
{
    header* block = (header*)mem;
    block->is_free = true;
}

void set_data_to_block(header *block, size_t sz)
{
    block->size = sz;
    block->is_free = false;
    block->next = NULL;

    active_size += sz;
    last_allocated = block + 1;
    char *end_of_data_block = (char *)to_end_data(block);

    if (max_heap < end_of_data_block)
    {
        max_heap = end_of_data_block;
    }
    strcpy(end_of_data_block, END_CHK);
}

header *to_start_metadata(header *block)
{
    return --block;
}

header *to_end_data(header *block)
{
    return (header *)((size_t)(block + 1) + block->size);
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
