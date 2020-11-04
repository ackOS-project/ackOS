#include "kernel/drivers/serial.h"
#include "kernel/logger.h"

void log_info(const char* module_name, const char* text)
{
    serial::print(COM1, "[INFO] ");
    serial::print(COM1, module_name);
    serial::print(COM1, ": ");

    serial::print(COM1, text);

    serial::putc(COM1, '\n');
}

void log_warn(const char* module_name, const char* text)
{
    serial::print(COM1, "[WARN] ");
    serial::print(COM1, module_name);
    serial::print(COM1, ": ");

    serial::print(COM1, text);

    serial::putc(COM1, '\n');
}

void log_error(const char* module_name, const char* text)
{
    serial::print(COM1, "[ERROR] ");
    serial::print(COM1, module_name);
    serial::print(COM1, ": ");

    serial::print(COM1, text);

    serial::putc(COM1, '\n');
}
