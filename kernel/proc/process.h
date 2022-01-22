#pragma once

#include "kernel/io/fd_table.h"

#include <sys/types.h>
#include <liback/utils/result.h>

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
    process* _parent_process;
    pid_t _pid;
    fd_table _fd_table;
    process_state _state = process_state::ZOMBIE;
    uintptr_t _memory_start = 0;
    uintptr_t _memory_end = 0;

public:
    pid_t get_pid() const { return _pid; }
    process* get_parent_pid() const { return _parent_process; }
    fd_table& get_fd_table() { return _fd_table; }
    const fd_table& get_fd_table() const { return _fd_table; }

    void set_memory_region(uintptr_t start, uintptr_t end)
    {
        _memory_start = start;
        _memory_end = end;
    }

    utils::result send_signal(int signal);

    friend process* process_start(process* parent, const char* name, void* entry_point);
};

process* process_get_from_pid(pid_t pid);

int process_destroy(process* process);

process* process_start(process* parent, const char* name, void* entry_point);

void processes_initialise();
