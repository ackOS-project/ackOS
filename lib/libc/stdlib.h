#pragma once

#include <stddef.h>
#include <stdint.h>

void* malloc(size_t size);

void* realloc(void* mem, size_t new_size);

void free(void* ptr);

int abs(int i);

long int strtol(const char* str, char** endptr, int base);
unsigned long strtoul(const char* str, char** endptr, int base);
long int atol(const char* str);
int atoi(const char* str);

