#pragma once

#include <sys/cdefs.h>
#include <cstdarg>
#include <cstddef>

__BEGIN_DECLS
void* malloc(size_t size);

void free(void* ptr);

int abs(int i);

char* itoa(int value, char* str, int base);

int sqrt(int x);

__END_DECLS
