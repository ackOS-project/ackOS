#pragma once

#include <sys/types.h>
#include <liback/utils/result.h>
#include "kernel/io/fd_table.h"

enum class process_state
{
    ZOMBIE,
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    STOPPED
};

class process
{
private:
    pid_t _parent_pid;
    pid_t _pid;
    fd_table _fd_table;
    process_state _state = process_state::ZOMBIE;

public:
    pid_t get_pid() { return _pid; }
    pid_t get_parent_pid() { return _parent_pid; }
    fd_table& get_fd_table() { return _fd_table; }

    utils::result send_signal(int signal);

    friend pid_t process_start(pid_t parent_pid, const char* name, void* entry_point);
};

process* process_get_from_pid(pid_t pid);

utils::result process_destroy(pid_t pid);

pid_t process_start(pid_t parent_pid, const char* name, void* entry_point);

void processes_initialise();
