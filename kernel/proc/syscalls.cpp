#include "kernel/mm/heap.h"
#include "kernel/proc/process.h"
#include "kernel/sys_info.h"

#include <liback/syscalls.h>

int syscalls_dispatch(process* proc, int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6)
{
    utils::result result = utils::result::ERR_INVALID_SYSCALL;

    if(call == ackos::SYSCALL_STREAM_READ)
    {
        int fd = *(int*)arg1;
        void* buff = *(void**)arg2;
        size_t size = *(size_t*)arg3;
        size_t* total_read = *(size_t**)arg4;

        result = proc->get_fd_table().read(fd, buff, size, total_read);
    }
    else if(call == ackos::SYSCALL_STREAM_WRITE)
    {
        int fd = *(int*)arg1;
        const void* buff = *(const void**)arg2;
        size_t size = *(size_t*)arg3;
        size_t* total_written = *(size_t**)arg4;

        result = proc->get_fd_table().write(fd, buff, size, total_written);
    }
    else if(call == ackos::SYSCALL_STREAM_OPEN)
    {
        result = utils::result::ERR_UNIMPLEMENTED;
    }
    else if(call == ackos::SYSCALL_STREAM_CLOSE)
    {
        int fd = *(int*)arg1;

        result = proc->get_fd_table().remove_node(fd);
    }
    else if(call == ackos::SYSCALL_STREAM_IOCALL)
    {
        int fd = *(int*)arg1;
        int request = *(int*)arg2;
        void* arg = *(void**)arg3;

        result = proc->get_fd_table().io_call(fd, request, arg);
    }
    else if(call == ackos::SYSCALL_STREAM_CLONE1)
    {
        int* new_fd = *(int**)arg1;
        int old_fd = *(int*)arg2;

        result = proc->get_fd_table().clone(new_fd, old_fd);
    }
    else if(call == ackos::SYSCALL_STREAM_CLONE2)
    {
        int old_fd = *(int*)arg1;
        int new_fd = *(int*)arg2;

        result = proc->get_fd_table().clone(new_fd, old_fd);
    }
    else if(call == ackos::SYSCALL_SYSTEM_GET_INFO)
    {
        ackos::system_info* info = *(ackos::system_info**)arg1;

        if(info == nullptr)
        {
            result = utils::result::ERR_INVALID_ADDRESS;
        }
        else
        {
            *info = get_system_info();

            result = utils::result::SUCCESS;
        }
    }
    else if(call == ackos::SYSCALL_MEMORY_ALLOC)
    {
        void** addr = *(void***)arg1;
        size_t size = *(size_t*)arg2;

        if(addr == nullptr)
        {
            result = utils::result::ERR_INVALID_ADDRESS;
        }
        else
        {
            *addr = heap_allocate(size);
        }

        if(*addr == nullptr)
        {
            result = utils::result::ERR_OUT_OF_MEMORY;
        }
        else
        {
            result = utils::result::SUCCESS;
        }
    }
    else if(call == ackos::SYSCALL_MEMORY_FREE)
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
