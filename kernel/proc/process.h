#pragma once

#include "kernel/io/fd_table.h"
#include "kernel/fs/filesystem.h"

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

class process_t
{
public:
    process_t* parent_proc;
    pid_t pid;
    fd_table_t fd_table;
    process_state state = process_state::ZOMBIE;
    uintptr_t page_table;

    utils::result send_signal(int signal);

    friend process_t* process_create(process_t* parent, const char* name, void* entry_point);
};

process_t* process_get_from_pid(pid_t pid);

int process_destroy(process_t* process);

process_t* process_create(process_t* parent, const char* name, void* entry_point);

void processes_initialise();
