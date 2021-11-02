#pragma once

#include <source_location>
#include <liback/utils/macros.h>

ATTRIBUTE_NO_RETURN void kpanic(const char* error, bool print_stacktrace = true, const std::source_location& location = std::source_location::current());
