#include "kernel/drivers/serial.h"
#include "kernel/logger.h"
#include <cstdio>

void vlog(log_level level, const char* module_name, const char* fmt, va_list args)
{
    const char* level_str;
    switch (level)
    {
    case log_level::INFO:
        level_str = "INFO";
        break;
    case log_level::WARN:
        level_str = "WARN";
        break;
    case log_level::ERROR:
        level_str = "ERROR";
        break;
    default:
        level_str = "UNKNOWN";
        break;
    }

    printf("[%s] %s: ", level_str, module_name);
    vprintf(fmt, args);
    printf("\n");
}

void log(log_level level, const char* module_name, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vlog(level, module_name, fmt, args);

    va_end(args);
}

void log_info(const char* module_name, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vlog(log_level::INFO, module_name, fmt, args);

    va_end(args);
}

void log_warn(const char* module_name, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vlog(log_level::WARN, module_name, fmt, args);

    va_end(args);
}

void log_error(const char* module_name, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vlog(log_level::ERROR, module_name, fmt, args);

    va_end(args);
}
