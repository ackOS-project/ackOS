#pragma once

#include <sys/cdefs.h>
#include <cstdarg>
#include <cstddef>
#include <cstdint>

__BEGIN_DECLS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void exit(int status);

void* malloc(size_t size);

void* realloc(void* mem, size_t new_size);

void free(void* ptr);

char* ltoa(int64_t value, char* str, int base);

char* ultoa(uint64_t value, char* str, int base);

char* utoa(uint32_t value, char* str, int base);

char* itoa(int value, char* str, int base);

int64_t strtol(const char* str, char** endptr, int base);

uint64_t strtoul(const char* str, char** endptr, int base);

uint32_t atol(const char* str);

int atoi(const char* str);

#ifndef BUILD_DISABLE_FPA
void ftoa(float value, char* str, int precision);

double pow(double x, double y);

#endif

int abs(int i);


int sqrt(int x);

__END_DECLS
