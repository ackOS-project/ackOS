#pragma once

#include <cstddef>
#include <sys/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t write(int fd, const void* buff, size_t count);

__END_DECLS
