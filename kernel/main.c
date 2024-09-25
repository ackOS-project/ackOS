#include <string.h>

#include "kernel/lib/log.h"
#include "kernel/mm/alloc.h"

int main(void)
{
#ifdef __ackos__
    kprintf("Hello, ackOS World!\n");
#endif

    //int* int_ptr = NULL;

    //*int_ptr = 42;

    return 0;
}
