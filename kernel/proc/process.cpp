#include <fcntl.h>
#include <csignal>
#include "kernel/dev/com.h"
#include "kernel/proc/process.h"

#define PROCESS_LIMIT 512

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
            result = process_destroy(get_pid());
        }

        break;
    }
    default:
        result = utils::result::ERR_INVALID_SIGNAL;

        break;
    }

    return result;
}

utils::result process_destroy(pid_t pid)
{
    utils::result result;

    if(processes[pid] != nullptr)
    {
        delete processes[pid];

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

pid_t process_start(pid_t parent_pid, const char* name, void* entry_point)
{
    pid_t pid = -1;

    for(int i = 0; i < PROCESS_LIMIT; i++)
    {
        if(processes[i] == nullptr)
        {
            processes[i] = new process;

            processes[i]->_parent_pid = parent_pid;
            processes[i]->_pid = i;
            pid = i;

            break;
        }
    }

    return pid;
}

void processes_initialise()
{
    pid_t pid = process_start(0, "kernel", nullptr);
    process* proc = process_get_from_pid(pid);

    proc->get_fd_table().insert_node_at(1, new com_port(1), O_WRONLY);
    proc->get_fd_table().insert_node_at(2, new com_port(1), O_WRONLY);
}
