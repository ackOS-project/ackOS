#pragma once

#include "kernel/proc/process.h"

int syscalls_dispatch(process* proc, int call, void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6);
