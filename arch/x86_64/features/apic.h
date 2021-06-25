#pragma once

#include <stdint.h>
#include <stddef.h>

namespace x86_64::apic
{
    bool check();

    void initialise();
}
