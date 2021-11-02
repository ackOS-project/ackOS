#include "sys/cdefs.h"
#include "stdlib/runtime/ubsan.h"

void __stdlib_heap_initialise();

void __stdlib_init()
{
    __stdlib_heap_initialise();
    __ubsan_set_output_file(stderr);
}
