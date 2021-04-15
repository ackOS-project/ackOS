#include <unistd.h>
#include <cerrno>
#include <liback/syscalls.h>

ssize_t write(int fd, const void* buff, size_t count)
{
    size_t written = 0;
    utils::result result = ackos::sys_wrapper::stream_write(fd, buff, count, &written);

    if(result == utils::result::ERR_INVALID_FD)
    {
        errno = EBADF;
    }
    else
    {
        errno = 0;
    }

    return (ssize_t)written;
}
