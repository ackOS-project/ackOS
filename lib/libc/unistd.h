#pragma once

#include <cstddef>
#include <sys/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

ssize_t read(int fd, void* buff, size_t count);
ssize_t write(int fd, const void* buff, size_t count);

int dup(int old_fd);
int dup2(int old_fd, int new_fd);

__END_DECLS
