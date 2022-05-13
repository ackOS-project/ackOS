#include <liback/syscalls.h>

#include "kernel/proc/syscalls.h"

namespace ackos
{
    utils::result syscall_dispatch(int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6)
    {
        // FIXME: hard coded pid
        utils::result result = syscalls_dispatch(process_get_from_pid(0), call, arg1, arg2, arg3, arg4, arg5, arg6);

        return result;
    }

    namespace sys_wrapper
    {
        utils::result stream_open(int* fd, const char* filename, int flags)
        {
            return syscall_dispatch(SYSCALL_STREAM_OPEN, &fd, &filename, &flags, NULL, NULL, NULL);
        }

        utils::result stream_close(int fd)
        {
            return syscall_dispatch(SYSCALL_STREAM_CLOSE, &fd, NULL, NULL, NULL, NULL, NULL);
        }

        utils::result stream_iocall(int fd, int request, void* arg)
        {
            return syscall_dispatch(SYSCALL_STREAM_IOCALL, &fd, &request, &arg, NULL, NULL, NULL);
        }

        utils::result stream_clone(int* new_fd, int old_fd)
        {
            return syscall_dispatch(SYSCALL_STREAM_CLONE1, &new_fd, &old_fd, NULL, NULL, NULL, NULL);
        }

        utils::result stream_clone(int old_fd, int new_fd)
        {
            return syscall_dispatch(SYSCALL_STREAM_CLONE2, &old_fd, &new_fd, NULL, NULL, NULL, NULL);
        }

        utils::result stream_read(int fd, void** buff, size_t size, size_t* total_read)
        {
            return syscall_dispatch(SYSCALL_STREAM_READ, &fd, &buff, &size, &total_read, NULL, NULL);
        }

        utils::result stream_write(int fd, const void* buff, size_t size, size_t* total_written)
        {
            return syscall_dispatch(SYSCALL_STREAM_WRITE, &fd, &buff, &size, &total_written, NULL, NULL);
        }

        utils::result stream_read(int fd, void* buff, size_t size, size_t* total_read)
        {
            return syscall_dispatch(SYSCALL_STREAM_READ, &fd, &buff, &size, &total_read, NULL, NULL);
        }

        utils::result system_get_info(system_info* info)
        {
            return syscall_dispatch(SYSCALL_SYSTEM_GET_INFO, &info, NULL, NULL, NULL, NULL, NULL);
        }
    }
}
