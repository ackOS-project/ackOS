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

FILE* fopen(const char *filename, const char *mode);

FILE* fdopen(int fd, const char* mode);

size_t fread(void* buff, size_t size, size_t count, FILE* file);

size_t fwrite(const void* buff, size_t size, size_t count, FILE* file);

int fgetc(FILE* file);

int fputc(int c, FILE* file);

int fputs(const char* str, FILE* file);

int getc(FILE* file);

int putc(int c, FILE* file);

int putchar(int c);

int getchar();

int puts(const char* str);

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args);

int vsprintf(char* buff, const char* fmt, va_list args); /* unsafe */

int sprintf(char* buff, const char* fmt, ...);

int snprintf(char* buff, size_t n, const char* fmt, ...);

int vfprintf(FILE* file, const char* fmt, va_list args);

int fprintf(FILE* file, const char* fmt, ...);

int vprintf(const char* fmt, va_list args);

int printf(const char* string, ...);

int vsscanf(const char* buffer, const char* format, va_list args);

int sscanf(const char* buffer, const char* format, ...);

__END_DECLS
