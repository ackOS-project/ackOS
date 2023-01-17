#pragma once

#include <stdint.h>
#include <stddef.h>

#define KiB * 0x400
#define MiB * 0x100000
#define GiB * 0x40000000

#define ATTR_PACKED __attribute__((__packed__))
#define ATTR_FORMAT(f, a, b)  __attribute__((format(f, a, b)))


#define MAX(X_, Y_) (((X_) > (Y_)) ? (X_) : (Y_))
#define MIN(X_, Y_) (((X_) < (Y_)) ? (X_) : (Y_))
#define ABS(X_) ((X_) < 0 ? -(X_) : (X_))

typedef intmax_t ssize_t;
