#pragma once

#include <liback/utils/result.h>
#include <liback/system_info.h>

#include <cstdarg>
#include <cstddef>

namespace ackos
{
    /* these aren't real syscalls but they allow us to implement proper syscalls later without re-writing most of our libaries */
    enum
    {
        SYSCALL_STREAM_READ,
        SYSCALL_STREAM_WRITE,
        SYSCALL_STREAM_OPEN,
        SYSCALL_STREAM_CLOSE,
        SYSCALL_STREAM_IOCALL,
        SYSCALL_STREAM_CLONE1,
        SYSCALL_STREAM_CLONE2,
        SYSCALL_SYSTEM_GET_INFO,
        SYSCALL_MEMORY_ALLOC,
        SYSCALL_MEMORY_FREE
    };

    utils::result syscall_dispatch(int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6);

    namespace sys_wrapper
    {
        utils::result stream_read(int fd, void* buff, size_t size, size_t* bytes_read);
        utils::result stream_write(int fd, const void* buff, size_t size, size_t* bytes_written);
        utils::result stream_open(int* fd, const char* filename, int flags); /* unimplemented */
        utils::result stream_close(int fd); /* also unimplemented */
        utils::result stream_iocall(int fd, int request, void* arg);
        utils::result stream_clone(int* new_fd, int old_fd);
        utils::result stream_clone(int old_fd, int new_fd);

        utils::result system_get_info(system_info* info);

        utils::result memory_allocate(void** addr, size_t size);
        utils::result memory_free(void* addr);
    }
}
