#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <sys/cdefs.h>
#include "kernel/arch/arch.h"

__BEGIN_DECLS

static constexpr const int page_size = 4096;

void* libmem_alloc_pages(int pages)
{
    return nullptr;
}

int libmem_free_pages(void* mem, int pages)
{
    //return munmap(mem, pages * page_size);

    return 0;
}

#define ALLOCATE_PAGES libmem_alloc_pages_safe
#define FREE_PAGES libmem_free_pages_safe
#define MAGIC_NUMBER 0xA110CA1E
#define HOW_MANY_PAGES 8

struct libmem_header_t
{
    uint32_t magic;
    size_t size;
    size_t allocated_size;

    libmem_header_t* last_split;

    libmem_header_t* prev;
    libmem_header_t* next;
};

static libmem_header_t* base_header;

enum libmem_err_t
{
    LIBMEM_SUCCESS,
    LIBMEM_ERR_NULL_HEADERS,
    LIBMEM_ERR_UNLINKED,
    LIBMEM_ERR_NO_MAGIC
};

static void libmem_print_error(libmem_err_t err)
{
    if(err == LIBMEM_SUCCESS)
    {
        return;
    }
    else if(err == LIBMEM_ERR_NULL_HEADERS)
    {
        fputs("heap error: cannot merge: headers are nullptr\n", stderr);
    }
    else if(err == LIBMEM_ERR_UNLINKED)
    {
        fputs("heap error: cannot merge: headers are not linked\n", stderr);
    }
    else if(err == LIBMEM_ERR_NO_MAGIC)
    {
        fputs("heap error: cannot merge: headers do not contain magic number\n", stderr);
    }
    else
    {
        fputs("heap error: error unknown\n", stderr);
    }

    arch::stacktrace_dump();
}

static libmem_header_t libmem_create_header(size_t size, size_t allocated_size, libmem_header_t* prev, libmem_header_t* next)
{
    return
    {
        .magic = MAGIC_NUMBER,
        .size = size,
        .allocated_size = allocated_size,
        .last_split = nullptr,
        .prev = prev,
        .next = next
    };
}

static libmem_header_t* libmem_split(libmem_header_t* header, size_t size)
{
    if(!header) return nullptr;

    size_t lhs_size = header->allocated_size - (size + sizeof(libmem_header_t));

    if(lhs_size < header->size || lhs_size > header->allocated_size || header->magic != MAGIC_NUMBER)
    {
        return nullptr;
    }

    header->allocated_size = lhs_size;

    libmem_header_t* split_header = (libmem_header_t*)(((uintptr_t)header) + sizeof(libmem_header_t) + lhs_size);
    libmem_header_t* last_split = header->last_split ? header->last_split : header;

    *split_header = libmem_create_header(size, size, last_split, nullptr);

    last_split->next = split_header;
    header->last_split = split_header;

    return split_header;
}

static void libmem_merge(libmem_header_t* header1, libmem_header_t* header2)
{
    libmem_err_t err = LIBMEM_SUCCESS;

    if(header1 == nullptr || header2 == nullptr)
    {
        err = LIBMEM_ERR_NULL_HEADERS;
    }
    else if(header1->next != header2 || header2->prev != header1)
    {
        err = LIBMEM_ERR_UNLINKED;
    }
    else if(header1->magic != MAGIC_NUMBER || header2->magic != MAGIC_NUMBER)
    {
        err = LIBMEM_ERR_NO_MAGIC;
    }

    if(err)
    {
        libmem_print_error(err);

        return;
    }

    header1->allocated_size += header2->allocated_size + sizeof(libmem_header_t);
    header1->next = header2->next;

    if(header1->prev)
    {
        header1->prev->last_split = header1->prev->last_split == header2 ? header1 : header1->prev->last_split;
    }
    else
    {
        header1->last_split = nullptr;
    }

    if(header1->next)
    {
        header1->next->prev = header1;
    }
}

static libmem_header_t* libmem_find_free_header(libmem_header_t* header, size_t size)
{
    libmem_header_t* hdr = header;

    if(!hdr)
    {
        return nullptr;
    }

    while(true)
    {
        if(hdr->size == 0 && hdr->allocated_size >= size)
        {
            hdr->size = size;

            return hdr;
        }

        hdr = libmem_split(hdr, size);

        if(hdr)
        {
            break;
        }
        else
        {
            fputs("heap error: cannot split: heap overflow detected\n", stderr);

            return nullptr;
        }
    }

    return hdr;
}

void* malloc(size_t size)
{
    return (void*)(((uintptr_t)libmem_find_free_header(base_header, size)) + sizeof(libmem_header_t));
}

void free(void* mem)
{
    libmem_header_t* header = (libmem_header_t*)((uintptr_t)mem - sizeof(libmem_header_t));

    if(header->magic != MAGIC_NUMBER)
    {
        return;
    }

    if(header->prev)
    {
        libmem_merge(header->prev, header);
    }
    else
    {
        header->size = 0;
    }
}

void* realloc(void* mem, size_t new_size)
{
    libmem_header_t* header = (libmem_header_t*)((uintptr_t)mem - sizeof(libmem_header_t));
    libmem_header_t* new_header = header;

    if(header->magic != MAGIC_NUMBER)
    {
        return nullptr;
    }

    if(new_size < header->allocated_size)
    {
        header->size = new_size;
    }
    else if(new_size > header->size)
    {
        new_header = libmem_find_free_header(base_header, new_size);

        memcpy((void*)((uintptr_t)new_header + sizeof(libmem_header_t)), mem, new_size);

        free(mem);
    }

    return (void*)((uintptr_t)new_header + sizeof(libmem_header_t));
}

static void libmem_init(void* mem, size_t size)
{
    base_header = (libmem_header_t*)mem;

    *base_header = libmem_create_header(0, size - sizeof(libmem_header_t), nullptr, nullptr);
}

__END_DECLS

#define HEAP_SIZE page_size * 64
static uint8_t heap_memory[HEAP_SIZE];

void __stdlib_heap_initialise()
{
    libmem_init((void*)heap_memory, HEAP_SIZE);
}
