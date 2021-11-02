#include "kernel/sys/logger.h"
#include <cstdio>

#define COLOUR_BLUE "\033[0;34m"
#define COLOUR_BLUE_BOLD "\033[1;34m"

#define COLOUR_YELLOW "\033[0;33m"
#define COLOUR_YELLOW_BOLD "\033[1;33m"

#define COLOUR_RED "\033[0;31m"
#define COLOUR_RED_BOLD "\033[1;31m"

#define COLOUR_RESET "\033[0m"

void vlog(log_level level, const char* module_name, const char* fmt, va_list args)
{
    const char* level_str;
    switch(level)
    {
    case log_level::INFO:
        level_str = COLOUR_BLUE "info" COLOUR_RESET;
        break;
    case log_level::WARN:
        level_str = COLOUR_YELLOW "warn" COLOUR_RESET;
        break;
    case log_level::ERROR:
        level_str = COLOUR_RED "error" COLOUR_RESET;
        break;
    default:
        level_str = "unknown";
        break;
    }

    printf("%s %s: ", level_str, module_name);
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
