#include <liback/syscalls.h>
#include "kernel/mm/heap.h"
#include "kernel/proc/process.h"

namespace ackos
{
    utils::result syscall_dispatch(int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6)
    {
        utils::result result = utils::result::ERR_INVALID_SYSCALL;

        if(call == SYSCALL_STREAM_READ)
        {
            int fd = *(int*)arg1;
            void* buff = *(void**)arg2;
            size_t size = *(size_t*)arg3;
            size_t* total_read = *(size_t**)arg4;

            // FIXME: hard coded pid
            result = process_get_from_pid(0)->get_fd_table().read(fd, buff, size, total_read);
        }
        else if(call == SYSCALL_STREAM_WRITE)
        {
            int fd = *(int*)arg1;
            const void* buff = *(const void**)arg2;
            size_t size = *(size_t*)arg3;
            size_t* total_written = *(size_t**)arg4;

            result = process_get_from_pid(0)->get_fd_table().write(fd, buff, size, total_written);
        }
        else if(call == SYSCALL_STREAM_OPEN)
        {
            result = utils::result::ERR_UNIMPLEMENTED;
        }
        else if(call == SYSCALL_STREAM_CLOSE)
        {
            result = utils::result::ERR_UNIMPLEMENTED;
        }
        else if(call == SYSCALL_STREAM_IOCALL)
        {
            int fd = *(int*)arg1;
            int request = *(int*)arg2;
            void* arg = *(void**)arg3;

            result = process_get_from_pid(0)->get_fd_table().io_call(fd, request, arg);
        }
        else if(call == SYSCALL_STREAM_CLONE1)
        {
            int* new_fd = *(int**)arg1;
            int old_fd = *(int*)arg2;

            result = process_get_from_pid(0)->get_fd_table().clone(new_fd, old_fd);
        }
        else if(call == SYSCALL_STREAM_CLONE2)
        {
            int old_fd = *(int*)arg1;
            int new_fd = *(int*)arg2;

            result = process_get_from_pid(0)->get_fd_table().clone(new_fd, old_fd);
        }
        else if(call == SYSCALL_MEMORY_ALLOC)
        {
            void** addr = *(void***)arg1;
            size_t size = *(size_t*)arg2;

            *addr = heap_allocate(size);
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
                heap_deallocate(addr);

                result = utils::result::SUCCESS;
            }
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

        utils::result stream_close(int fd)
        {
            return syscall_dispatch(SYSCALL_STREAM_OPEN, &fd, NULL, NULL, NULL, NULL, NULL);
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
    }
}
