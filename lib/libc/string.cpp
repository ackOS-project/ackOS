#include <string.h>

size_t strlen(const char* str) 
{
    size_t len = 0;
    while (str[len])
    {
        len++;
    }

    return len;
}

void* memcpy(void *dest, const void *src, size_t len)
{
    size_t i;

    if((uintptr_t)dest % sizeof(long) == 0 &&
        (uintptr_t)src % sizeof(long) == 0 &&
        len % sizeof(long) == 0)
    {

        long* d = (long int*)dest;
        const long *s = (const long int*)src;

        for (i = 0; i < len / sizeof(long); i++)
        {
            d[i] = s[i];
        }
    }
    else
    {
        char* d = (char*)dest;
        const char* s = (const char*)src;

        for (i = 0; i < len; i++)
        {
            d[i] = s[i];
        }
    }

    return dest;
}

void* memset(void *ptr, int value, size_t num)
{
    uint8_t* p = (uint8_t*)ptr;

	while (num--)
    {
        *p++ = (uint8_t)value;
    }

	return ptr;
}

char* strcpy(char *dest, const char *src)
{
    return (char*)memcpy(dest, src, strlen(src) + 1);
}
