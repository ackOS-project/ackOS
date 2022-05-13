#pragma once

#include <source_location>
#include <liback/utils/macros.h>

ATTRIBUTE_NO_RETURN void kpanic(const char* error, void* stacktrace_addr = nullptr, const std::source_location& location = std::source_location());
