#pragma once

#define KiB * 0x400
#define MiB * 0x100000
#define GiB * 0x40000000

#define ATTR_PACKED __attribute__((__packed__))

#define MAX(X_, Y_) (((X_) > (Y_)) ? (X_) : (Y_))
#define MIN(X_, Y_) (((X_) < (Y_)) ? (X_) : (Y_))
