#include <cstdlib>
#include <cstring>
#include <liback/syscalls.h>

void* malloc(size_t size)
{
    void* mem;
    ackos::sys_wrapper::memory_allocate(&mem, size);

    return mem;
}

void* calloc(size_t size)
{
    void* mem = malloc(size);
    memset(mem, 0, size);

    return mem;
}

void free(void* ptr)
{
    ackos::sys_wrapper::memory_free(ptr);
}

char* ltoa(int64_t value, char* str, int base)
{
    char* rc;
    char* ptr;
    char* low;

    if(base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;

    if(value < 0 && base == 10)
    {
        *ptr++ = '-';
    }

    low = ptr;
    do
    {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    }
    while(value);

    *ptr-- = '\0';

    while(low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}

char* ultoa(uint64_t value, char* str, int base)
{
    char* rc;
    char* ptr;
    char* low;

    if(base < 2 || base > 36)
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;

    if(value < 0 && base == 10)
    {
        *ptr++ = '-';
    }

    low = ptr;
    do
    {
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    }
    while(value);

    *ptr-- = '\0';

    while(low < ptr)
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }

    return rc;
}

char* utoa(uint32_t value, char* str, int base)
{
    return ultoa((uint64_t)value, str, base);
}

char* itoa(int value, char* str, int base)
{
    return ultoa((uint64_t)value, str, base);
}

int abs(int i)
{
    return i < 0 ? -i : i;
}

int sqrt(int x)
{
    int quotient = 0;
    int i = 0;

    bool answer_found = false;
    while(answer_found == false)
    {
        if(i * i == x)
        {
          quotient = i;
          answer_found = true;
        }

        i++;
    }

    return quotient;
}
