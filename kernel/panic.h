#pragma once

#include <source_location>

void kpanic(const char* error, const std::source_location& location = std::source_location::current());
