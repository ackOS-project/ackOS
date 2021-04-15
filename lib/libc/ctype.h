#pragma once

#include <sys/cdefs.h>
#include <cstdarg>
#include <cstddef>

__BEGIN_DECLS
int isalpha(int c);

int isspace(int c);

int isdigit(int c);

int isupper(int c);

int islower(int c);

int isprint(int c);

int isalnum(int c);

int iscntrl(int c);

int isxdigit(int c);

int tolower(int c);

int toupper(int c);

__END_DECLS
