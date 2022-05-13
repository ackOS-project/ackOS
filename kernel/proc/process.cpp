#include "kernel/dev/early_console.h"
#include "kernel/arch/x86_64/feat/com_dev.h"
#include "kernel/proc/process.h"

#include <fcntl.h>
#include <stdlib/runtime/ubsan.h>
#include <csignal>

#define PROCESS_LIMIT 512

extern uintptr_t _kernel_start;
extern uintptr_t _kernel_end;

static process* processes[PROCESS_LIMIT];

utils::result process::send_signal(int signal)
{
    utils::result result;

    switch(signal)
    {
    case SIGKILL:
    {
        if(get_pid() == 0) /* the kernel */
        {
            result = utils::result::ERR_ACCESS_DENIED;
        }
        else
        {
            _state = process_state::ZOMBIE;
            result = process_destroy(this);
        }

        break;
    }
    default:
        result = utils::result::ERR_INVALID_SIGNAL;

        break;
    }

    return result;
}

int process_destroy(process* proc)
{
    int result;

    if(proc != nullptr)
    {
        delete proc;

        result = utils::result::SUCCESS;
    }
    else
    {
        result = utils::result::ERR_INVALID_PID;
    }

    return result;
}

process* process_get_from_pid(pid_t pid)
{
    return processes[pid];
}

process* process_start(process* parent, const char* name, void* entry_point)
{
    process* proc = nullptr;

    for(int i = 0; i < PROCESS_LIMIT; i++)
    {
        if(processes[i] == nullptr)
        {
            processes[i] = new process;

            processes[i]->_parent_process = parent;
            processes[i]->_pid = i;
            proc = processes[i];

            break;
        }
    }

    return proc;
}

void processes_initialise()
{
    process* kernel_proc = process_start(nullptr, "kernel", nullptr);

    kernel_proc->set_memory_region(_kernel_start, _kernel_end);

    kernel_proc->get_fd_table().insert_node_at(0, new early_console_device(), O_RDONLY);
    kernel_proc->get_fd_table().insert_node_at(1, new early_console_device(), O_WRONLY);
    kernel_proc->get_fd_table().insert_node_at(2, new early_console_device(), O_WRONLY);
    kernel_proc->get_fd_table().insert_node_at(3, new com_port(2), O_WRONLY);

    FILE* log_stream = fdopen(3, "w");
    fputs("This is the log file. Verbose information will appear here. \n\n", log_stream);
    __ubsan_set_output_file(log_stream);
}
