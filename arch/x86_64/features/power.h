#pragma once

/* shutdown and reboot functions for qemu and VirtualBox */

namespace x86_64
{
    void reboot_8042();

    void shutdown_virtual_machine();
}
