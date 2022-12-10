#pragma once

#include <stdarg.h>

void kputs(const char* msg);

int kvprintf(const char* fmt, va_list args);
int kprintf(const char* fmt, ...);
