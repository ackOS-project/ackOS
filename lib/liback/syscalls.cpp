#include <liback/syscalls.h>
#include "kernel/mm/memory.h"
#include "kernel/drivers/serial.h"
#include "kernel/io/fdm.h"

namespace ackos
{
    utils::result syscall_dispatch(int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6)
    {
        utils::result result = utils::result::ERR_INVALID_ARGUMENT;

        if(call == SYSCALL_MEMORY_ALLOC)
        {
            void** addr = *(void***)arg1;
            size_t size = *(size_t*)arg2;

            *addr = memory_alloc(size);
            if(addr == nullptr)
            {
                result = utils::result::ERR_OUT_OF_MEMORY;
            }
            else
            {
                result = utils::result::SUCCESS;
            }
        }
        else if(call == SYSCALL_MEMORY_FREE)
        {
            void* addr = *(void**)arg1;

            if(addr == nullptr)
            {
                result = utils::result::ERR_INVALID_ADDRESS;
            }
            else
            {
                memory_free(addr);

                result = utils::result::SUCCESS;
            }
        }
        else if(call == SYSCALL_STREAM_OPEN)
        {
            result = utils::result::ERR_UNIMPLEMENTED;
        }
        else if(call == SYSCALL_STREAM_READ)
        {
            result = utils::result::ERR_UNIMPLEMENTED;
        }
        else if(call == SYSCALL_STREAM_WRITE)
        {
            int fd = *(int*)arg1;
            const void* buff = *(const void**)arg2;
            size_t size = *(size_t*)arg3;
            size_t* total_written = *(size_t**)arg4;

            result = fdm::write(fd, buff, size, total_written);
        }

        return result;
    }

    namespace sys_wrapper
    {
        utils::result memory_allocate(void** addr, size_t size)
        {
            return syscall_dispatch(SYSCALL_MEMORY_ALLOC, &addr, &size, NULL, NULL, NULL, NULL);
        }

        utils::result memory_free(void* addr)
        {
            return syscall_dispatch(SYSCALL_MEMORY_FREE, &addr, NULL, NULL, NULL, NULL, NULL);
        }

        utils::result stream_open(int* fd, const char* filename, int flags)
        {
            return syscall_dispatch(SYSCALL_STREAM_OPEN, &fd, &filename, &flags, NULL, NULL, NULL);
        }

        utils::result stream_read(int fd, void** buff, size_t size, size_t* total_read)
        {
            return syscall_dispatch(SYSCALL_STREAM_READ, &fd, &buff, &size, &total_read, NULL, NULL);
        }

        utils::result stream_write(int fd, const void* buff, size_t size, size_t* total_written)
        {
            return syscall_dispatch(SYSCALL_STREAM_WRITE, &fd, &buff, &size, &total_written, NULL, NULL);
        }
    }
}