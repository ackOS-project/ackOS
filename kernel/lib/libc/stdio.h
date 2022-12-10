#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

int sprintf(char* buff, const char* fmt, ...);

int snprintf(char* buff, size_t n, const char* fmt, ...);

int vsprintf(char* buff, const char* fmt, va_list args);

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args);
