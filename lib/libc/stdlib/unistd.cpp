#include <unistd.h>
#include <cstdarg>
#include <cerrno>
#include <liback/syscalls.h>

#include <cstdio>

int open(const char* path, int flags, ...)
{
    va_list args;
    va_start(args, flags);

    int fd;

    utils::result result = ackos::sys_wrapper::stream_open(&fd, path, flags);

    errno = 0;

    if(!result)
    {
        if(result == utils::result::ERR_NO_SUCH_FILE_OR_DIRECTORY)
        {
            errno = ENOENT;
        }
        else if(result == utils::result::ERR_UNIMPLEMENTED)
        {
            errno = ENOSYS;
        }

        return -1;
    }

    va_end(args);

    return fd;
}

ssize_t read(int fd, void* buff, size_t count)
{
    size_t read = 0;
    utils::result result = ackos::sys_wrapper::stream_read(fd, buff, count, &read);

    if(result == utils::result::ERR_INVALID_FD || result == utils::result::ERR_OUT_OF_BOUNDS)
    {
        errno = EBADF;

        return -1;
    }
    else if(result == utils::result::ERR_NOT_READABLE || result == utils::result::ERR_UNIMPLEMENTED)
    {
        errno = EINVAL;

        return -1;
    }
    else if(result == utils::result::ERR_INVALID_ADDRESS)
    {
        errno = EFAULT;

        return -1;
    }
    else
    {
        errno = 0;
    }

    return (ssize_t)read;
}

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

int dup(int old_fd)
{
    int new_fd = -1;

    utils::result result = ackos::sys_wrapper::stream_clone(old_fd, 0, &new_fd, false);

    switch((int)result)
    {
    case utils::result::ERR_INVALID_FD:
    {
        errno = EBADF;

        break;
    }
    case utils::result::ERR_OUT_OF_BOUNDS:
    {
        errno = EBADF;

        break;
    }
    case utils::result::ERR_INVALID_ARGUMENT:
    {
        errno = EINVAL;

        break;
    }
    default:
        errno = 0;

        break;
    }

    return new_fd;
}

int dup2(int old_fd, int new_fd)
{
    int res_fd = -1;

    utils::result result = ackos::sys_wrapper::stream_clone(old_fd, new_fd, &res_fd, true);

    switch((int)result)
    {
    case utils::result::ERR_INVALID_FD:
    {
        errno = EBADF;
        return -1;

        break;
    }
    case utils::result::ERR_OUT_OF_BOUNDS:
    {
        errno = EBADF;
        return -1;

        break;
    }
    case utils::result::ERR_INVALID_ARGUMENT:
    {
        errno = EINVAL;
        return -1;

        break;
    }
    default:
        errno = 0;

        break;
    }

    return res_fd;
}

void _exit(int status)
{
    // FIXME

    asm volatile("cli");

    while(true)
    {
        asm volatile("hlt");
    }
}
