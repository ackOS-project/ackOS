#pragma once

#define MASTER_PIC 0x20
#define MASTER_PIC_COMMAND 0x20
#define MASTER_PIC_DATA (MASTER_PIC + 1)

#define SLAVE_PIC 0xA0
#define SLAVE_PIC_COMMAND 0xA0
#define SLAVE_PIC_DATA (SLAVE_PIC + 1)

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW1_8086 0x01

void pic8259_remap();

void pic8259_mask();

void pic8259_disable();

void pic8259_finish_interrupt();
