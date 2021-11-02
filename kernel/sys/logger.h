#pragma once

enum class log_level
{
    INFO,
    WARN,
    ERROR
};

void log(log_level level, const char* module_name, const char* fmt, ...);

void log_info(const char* module_name, const char* fmt, ...);
void log_warn(const char* module_name, const char* text, ...);
void log_error(const char* module_name, const char* text, ...);
