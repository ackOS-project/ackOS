#pragma once

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21

#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1

#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW1_8086 0x01

void pic8259_remap();

void pic8259_mask();

void pic8259_disable();

void pic8259_master_eoi();

void pic8259_slave_eoi();
