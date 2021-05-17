#pragma once

#include <sys/cdefs.h>
#include <cstdarg>
#include <cstddef>
#include <cstdint>

__BEGIN_DECLS
void* malloc(size_t size);

void free(void* ptr);

int abs(int i);

char* ltoa(int64_t value, char* str, int base);

char* ultoa(uint64_t value, char* str, int base);

char* utoa(uint32_t value, char* str, int base);

char* itoa(int value, char* str, int base);

int sqrt(int x);

__END_DECLS
