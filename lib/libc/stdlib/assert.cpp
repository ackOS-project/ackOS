#include <assert.h>
#include <cstdio>
#include <cstdlib>

void _assert_fail(const char* expression, const char* filename, const char* function, int line)
{
    fprintf(stderr, "%s() in %s:%d: assertion for '%s' failed\n", function, filename, line, expression);

    exit(EXIT_FAILURE);

    while(true);
}