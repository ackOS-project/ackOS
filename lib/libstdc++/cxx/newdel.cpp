#include <cstdlib>
#include <cstdio>

static void check_addr(void* addr)
{
    if(addr == nullptr)
    {
        fputs("fatal error: void *operator new(size_t) returned nullptr\n", stderr);

        exit(EXIT_FAILURE);
    }
}

void *operator new(size_t size)
{
    void* addr = malloc(size);

    check_addr(addr);

    return addr;
}
 
void *operator new[](size_t size)
{
    void* addr = malloc(size);

    check_addr(addr);

    return addr;
}
 
void operator delete(void* ptr)
{
    free(ptr);
}
 
void operator delete[](void* ptr)
{
    free(ptr);
}

/* sized deallocation functions */
void operator delete(void* ptr, size_t size)
{
    free(ptr);
}
 
void operator delete[](void* ptr, size_t size)
{
    free(ptr);
}
