#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "kernel/lib/util.h"

// A simple stack based heap with a free list

#define HEAP_SIZE 10 KiB

static uint8_t heap_data[HEAP_SIZE];
static void* heap_ptr = heap_data;


#define FREE_LIST_ENTRY_COUNT 30

static void* free_list[FREE_LIST_ENTRY_COUNT] = {NULL};
static size_t free_list_size = 0;

void* malloc(size_t size)
{
    if(((size_t)heap_ptr + sizeof(size_t) + size) >= (size_t)heap_data + HEAP_SIZE)
    {
        // if the heap pointer exceeds the heap size (i.e the heap has reached its limit), then we will
        // tediously iterate through the free list for a suitable place for the memory allocation
        for(size_t i = 0; i < free_list_size; i++)
        {
            if(free_list[i] == NULL)
            {
                continue;
            }

            size_t sz = *(size_t*)free_list[i];

            if(sz >= size)
            {
                free_list[i] = NULL;

                return free_list[i] + sizeof(size_t);
            }
        }
    }

    void* addr = heap_ptr;
    size_t* sz = (size_t*)addr;

    heap_ptr = (void*)(uintptr_t)heap_ptr + sizeof(size_t) + size;

    *sz = size;
    
    return (void*)(uintptr_t)addr + sizeof(size_t);
}

void* realloc(void* ptr, size_t size)
{
    size_t old_size = *(size_t*)(uintptr_t)ptr - sizeof(size_t);

    if(old_size <= size)
    {
        return ptr;
    }

    void* mem = malloc(size);

    memcpy(mem, (void*)(uintptr_t)ptr + sizeof(size_t), old_size);

    free(ptr);

    return mem;
}

void free(void* ptr)
{
    if(ptr < (void*)heap_data || ptr > (void*)(heap_data + HEAP_SIZE))
    {
        // error double free/corruption

        return;
    }
    if(free_list_size + 1 >= FREE_LIST_ENTRY_COUNT)
    {
        for(size_t i = 0; i < FREE_LIST_ENTRY_COUNT; i++)
        {
            if(free_list[i] == NULL)
            {
                free_list[i] = ptr - sizeof(size_t);
            }
        }

        // error: cannot free
    }

    free_list[free_list_size++] = ptr - sizeof(size_t);
}

//////////////////
static const char strtoint_lookup_table[] = "0123456789abcdefghijklmnopqrstuvwxyz";

static int strtoint_lut_binary_search(char d, int low, int high)
{
    if(low > high)
    {
        return -1;
    }
    
    int mid = low + (high - low) / 2;

    if(strtoint_lookup_table[mid] == d)
    {
        return mid;
    }
    else if(strtoint_lookup_table[mid] > d)
    {
        return strtoint_lut_binary_search(d, low, mid - 1);
    }
        
    return strtoint_lut_binary_search(d, mid + 1, high); 
}

static int strtoint_lut_find(char d, int base)
{
    return strtoint_lut_binary_search(tolower(d), 0, base - 1);
}

long int strtol(const char* str, char** endptr, int base)
{
    long int res = 0;
    size_t len = strlen(str);
    char* endp = NULL;

    if(endptr == NULL)
    {
        endptr = &endp;
    }

    *endptr = (char*)str;

    if(len == 0)
    {
        return 0;
    }

    if(base < 1 || (size_t)base > (sizeof(strtoint_lookup_table) / sizeof(char)))
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], base);

        if(d < 0) break;

        res = (res * base) + d;

        (*endptr)++;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

/*
enable when a testing framework has been established

TEST test_strtol()
{
    char* endp = NULL;
    const char* str = "1234abc";
    CHECK(strtol(str, &endp, 10) == 1234);
    CHECK(endp[0] == str[4]);
}

TEST test_strtol_neg_sign()
{
    char* endp = NULL;
    const char* str = "-1234";
    CHECK(strtol(str, &endp, 10) == -1234);
}

TEST test_strtol_pos_sign()
{
    char* endp = NULL;
    const char* str = "+1234";
    CHECK(strtol(str, &endp, 10) == 1234);
}

TEST test_strtol_null_endptr()
{
    const char* str = "1234";
    CHECK(strtol(str, NULL, 10) == 1234);
}

TEST test_strtol_hex()
{
    char* endp = NULL;
    const char* str = "FEDCBA09";
    CHECK(strtol(str, &endp, 16) == 0xFEDCBA09);
}

TEST test_strtol_termination()
{
    char* endp = NULL;
    const char* str = "123 241";
    CHECK(strtol(str, &endp, 16) == 123);
}
*/

unsigned long int strtoul(const char* str, char** endptr, int base)
{
    unsigned long int res = 0;
    size_t len = strlen(str);
    char* endp = NULL;

    if(endptr == NULL)
    {
        endptr = &endp;
    }

    *endptr = (char*)str;

    if(len == 0)
    {
        return 0;
    }

    if(base < 1 || (size_t)base > (sizeof(strtoint_lookup_table) / sizeof(char)))
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], base);

        if(d < 0) break;

        res = (res * base) + d;

        (*endptr)++;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

long int atol(const char* str)
{
    return strtol(str, NULL, 10);
}

int atoi(const char* str)
{
    unsigned long int res = 0;
    size_t len = strlen(str);

    if(len == 0)
    {
        return 0;
    }

    size_t i = str[0] == '-' || str[0] == '+' ? 1 : 0;

    for(; i < len; i++)
    {
        int d = strtoint_lut_find(str[i], 10);

        if(d < 0) break;

        res = (res * 10) + d;
    }

    if(str[0] == '-')
    {
        res = -res;
    }

    return res;
}

int abs(int i)
{
    return i < 0 ? -i : i;
}

