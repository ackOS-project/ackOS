#pragma once

#include <stdarg.h>

#include "kernel/lib/util.h"

#define KERN_INFO "\e\x01"
#define KERN_WARN "\e\x02"
#define KERN_PANIC "\e\x03"
#define KERN_DEBUG "\e\x04"

void kputs(const char* msg);

int kvprintf(const char* fmt, va_list args);

/*
* supported format specifiers:
*
* %    - prints a literal '%' character
* c    - prints a single character
* s    - prints a string
* i, d - prints a signed integer
* u    - prints an unsigned integer
* o    - prints an unsigned integer in octal
* x    - prints an unsigned integer in hexadecimal
* X    - prints an unsigned integer in hexadecimal and uses uppercase digits
* p    - prints an address of type void*
*
* supported size modifiers:
*
* ll - changes the type to long long int (uint64_t)
* l  - changes the type to long int (uint64_t)
* h  - changes the type to short (uint16_t)
* hh - changes the type to char (uint8_t)
* z  - changes the type to size_t
*
* supported format modifiers:
* '-x' - justify text to the right x many spaces at maximum
* '0x' - place x many 0s before a number at maximum
* '+'  - always show the positive sign on positive numbers
* '#'  - alternative form; for the o, x, X specifiers this modifier prepends a '0o' or '0x' repectively.
*/
ATTR_FORMAT(printf, 1, 2) int kprintf(const char* fmt, ...);

void kpanic(void);
