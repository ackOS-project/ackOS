#include <sys/cdefs.h>
#include <sys/runtime/ubsan.h>

void __stdlib_init()
{
    __ubsan_set_output_file(stderr);
}
