#pragma once

namespace x86_64
{
    void pic8259_remap();

    void pic8259_mask();

    void pic8259_disable();

    void pic8259_master_eoi();

    void pic8259_slave_eoi();
}
