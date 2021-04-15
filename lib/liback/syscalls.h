#pragma once

#include <liback/utils/result.h>
#include <cstdarg>
#include <cstddef>

namespace ackos
{
    /* these aren't real syscalls but they allow us to implement proper syscalls later without re-writing most of our libaries */
    enum
    {
        SYSCALL_MEMORY_ALLOC,
        SYSCALL_MEMORY_FREE,
        SYSCALL_STREAM_OPEN,
        SYSCALL_STREAM_READ,
        SYSCALL_STREAM_WRITE
    };

    utils::result syscall_dispatch(int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6);

    namespace sys_wrapper
    {
        utils::result memory_allocate(void** addr, size_t size);
        utils::result memory_free(void* addr);

        utils::result stream_open(int* fd, const char* filename, int flags); /* unimplemented */
        utils::result stream_read(int fd, void* buff, size_t size, size_t* bytes_read); /* also unimplemented */
        utils::result stream_write(int fd, const void* buff, size_t size, size_t* bytes_written);
    }
}
