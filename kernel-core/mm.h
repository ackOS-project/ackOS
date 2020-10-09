#pragma once
// Memory Manager


typedef struct heap_block
{
    struct heap_block* next;
    size_t size;
    bool isfree;
    int x; // for alligment
} header;
void set_data_to_block(header* block, size_t sz);
header* to_end_data(header* block);

//#define Heap_Capacity 2000000 // Lots
#define Heap_Capacity 10000 // Not much
unsigned long long active_size;  // # bytes in active allocations

static uint64_t heap[Heap_Capacity/sizeof(uint64_t)];

static char* max_heap =(char*)(&heap[0]);
static char* min_heap = (char*)(&heap[0]);
static char *END_CHK = "\xDE\xAD\xC0\xDA";
header* last_allocated;

char* strcpy(char * dest_ptr, const char * src_ptr)
{
	char* strresult = dest_ptr;
	if((NULL != dest_ptr) && (NULL != src_ptr))
	{
	  /* Start copy src to dest */
	  while (NULL != *src_ptr)
	  {
	    *dest_ptr++ = *src_ptr++;
	  }
	  /* NULL termination */
	  *dest_ptr = NULL;
	}
	return strresult;

}
void* malloc(size_t sz)
{
    if(sz == 0 || sz > Heap_Capacity)
    {
        return NULL;
    }

    header* block = (header*)heap;
    if(active_size == 0)
    {
        set_data_to_block(block, sz);
        return (void*)last_allocated;
    }

    while(block->next != NULL)
    {
        block = block->next;
    }

    block->next = (header*)((char*)to_end_data(block) + 8);
    header* new_block = block->next;
    set_data_to_block(new_block, sz);

    return (void*)last_allocated;
}

template<typename T>
void free(T* &mem)
{
    header* block = (header*)mem;
    block->isfree = true;
    // heap[(int*)mem] = 0;
    *mem = 0;
    mem = (T*)0x0;
}

void set_data_to_block(header* block, size_t sz)
{
    block->size = sz;
    block->isfree = false;
    block->next = NULL;

    active_size += sz;
    last_allocated = block + 1;
    char* end_of_data_block = (char*)to_end_data(block);

    if(max_heap < end_of_data_block)
    {
        max_heap = end_of_data_block;
    }
    strcpy(end_of_data_block, END_CHK);
}

header* to_start_metadata(header* block)
{
    return --block;
}

header* to_end_data(header* block)
{
    return (header*)((size_t)(block+1) + block->size);
}
