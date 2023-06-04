#pragma once

#include <stdarg.h>

#include <liback/util.h>

#define KERN_INFO "\e\x01"
#define KERN_WARN "\e\x02"
#define KERN_PANIC "\e\x03"
#define KERN_DEBUG "\e\x04"

#define TERM_RESET_COLOUR "\e[0m"
#define TERM_RED "\e[0;31m"
#define TERM_GREEN "\e[0;32m"
#define TERM_YELLOW "\e[0;33m"
#define TERM_BLUE "\e[0;34m"
#define TERM_MAGENTA "\e[0;35m"
#define TERM_CYAN "\e[0;36m"

#define TERM_RED_BG "\e[0;41m"
#define TERM_GREEN_BG "\e[0;42m"
#define TERM_YELLOW_BG "\e[0;43m"
#define TERM_BLUE_BG "\e[0;44m"
#define TERM_MAGENTA_BG "\e[0;45m"
#define TERM_CYAN_BG "\e[0;46m"

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
* '-x' - justify text to the right at least x many spaces
* '0x' - place at least x many 0s before a number
* '+'  - always show the positive sign on positive numbers
* '#'  - alternative form; for the o, x, X specifiers this modifier prepends a '0o' or '0x' repectively.
* `'`  - places spaces in between digits to separate thousands, millions places, etc. When using the 'z' size specifier, it will also quantise the value with a unit such as KiB, MiB, GiB
*/
ATTR_FORMAT(printf, 1, 2) int kprintf(const char* fmt, ...);

void kpanic(void);

#ifdef BUILD_CONFIG_DEBUG
#define EXPECT(EXPR_) (void)((EXPR_) ? 0 : kprintf(KERN_PANIC "ASSERTION FAILED: %s\n", #EXPR_))
#else
#define EXPECT(EXPR_) (void)0
#endif
