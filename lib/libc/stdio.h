#pragma once

#include <sys/cdefs.h>
#include <cerrno>
#include <cstdarg>
#include <cstddef>

__BEGIN_DECLS
struct FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#define EOF -1

size_t fwrite(const void* buff, size_t size, size_t count, FILE* file);

int fputc(int c, FILE* file);

int fputs(const char* str, FILE* file);

int putc(int c, FILE* file);

int putchar(int c);

int puts(const char* str);

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args);

int vsprintf(char* buff, const char* fmt, va_list args); /* unsafe */

int sprintf(char* buff, const char* fmt, ...);

int snprintf(char* buff, size_t n, const char* fmt, ...);

int vfprintf(FILE* file, const char* fmt, va_list args);

int fprintf(FILE* file, const char* fmt, ...);

int vprintf(const char* fmt, va_list args);

int printf(const char* string, ...);

__END_DECLS
