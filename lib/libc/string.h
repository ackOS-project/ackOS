#pragma once

#include <stddef.h>
#include <stdint.h>

void* memcpy(void *dest, const void *src, size_t len);

void* memset(void *ptr, int value, size_t num);

char* strcpy(char *dest, const char *src);

size_t strlen(const char* str);

