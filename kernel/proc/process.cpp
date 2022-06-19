#include "kernel/dev/early_console.h"
#include "kernel/arch/x86_64/feat/com_dev.h"
#include "kernel/proc/process.h"

#include <fcntl.h>
#include <stdlib/runtime/ubsan.h>
#include <csignal>
#include <vector>

extern uintptr_t _kernel_start;
extern uintptr_t _kernel_end;

static std::vector<process_t*> process_list;
static pid_t _next_pid = 0;

utils::result process_t::send_signal(int signal)
{
    utils::result result;

    switch(signal)
    {
    case SIGKILL:
    {
        if(pid == 0) /* the kernel */
        {
            result = utils::result::ERR_ACCESS_DENIED;
        }
        else
        {
            state = process_state::ZOMBIE;
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

int process_destroy(process_t* proc)
{
    int result;

    if(proc != nullptr)
    {
        _next_pid = proc->pid;

        delete proc;

        result = utils::result::SUCCESS;
    }
    else
    {
        result = utils::result::ERR_INVALID_ADDRESS;
    }

    return result;
}

process_t* process_get_from_pid(pid_t pid)
{
    for(auto& proc : process_list)
    {
        if(proc->pid == pid)
        {
            return proc;
        }
    }

    return nullptr;
}

process_t* process_create(process_t* parent, const char* name, void* entry_point)
{
    process_t* proc = new process_t;

    proc->parent_proc = parent;
    proc->pid = _next_pid;

    if(parent != nullptr)
    {
        proc->fd_table = parent->fd_table;
    }

    process_list.push_back(proc);

    _next_pid = process_list.size();

    return proc;
}

void processes_initialise()
{
    process_t* kernel_proc = process_create(nullptr, "kernel", nullptr);

    kernel_proc->fd_table.insert_node_at(0, new early_console_device());
    kernel_proc->fd_table.insert_node_at(1, new early_console_device());
    kernel_proc->fd_table.insert_node_at(2, new early_console_device());

    kernel_proc->fd_table.insert_node_at(3, new com_port(2));

    FILE* log_stream = fdopen(3, "w");
    fputs("This is the log file. Verbose information will appear here. \n\n", log_stream);
    __ubsan_set_output_file(log_stream);
}
