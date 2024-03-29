#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define KiB * 0x400
#define MiB * 0x100000
#define GiB * 0x40000000L
#define TiB * 0x10000000000L
#define PiB * 0x4000000000000L

#define ATTR_PACKED __attribute__((__packed__))
#define ATTR_ALIGN(a) __attribute__((aligned(a)))
#define ATTR_FORMAT(f, a, b)  __attribute__((format(f, a, b)))

#define MAX(X_, Y_) (((X_) > (Y_)) ? (X_) : (Y_))
#define MIN(X_, Y_) (((X_) < (Y_)) ? (X_) : (Y_))
#define ABS(X_) ((X_) < 0 ? -(X_) : (X_))
#define DIV_UP(X_, Y_) (((X_) % (Y_)) == 0 ? (X_) / (Y_) : (X_) / (Y_) + 1)

#define STATIC_LEN(ARR_) (sizeof((ARR_)) / sizeof((ARR_)[0]))

typedef intmax_t ssize_t;
