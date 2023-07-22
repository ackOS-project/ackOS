#pragma once

#include "liback/util.h"

struct ATTR_PACKED io_apic_interrupt_redirect
{
    union ATTR_PACKED
    {
        struct ATTR_PACKED
        {
            uint64_t vector : 8, /* Allowed values are from 0x10 to 0xFE. */
                     delivery_mode : 3, /* 0 = Normal,
                                        1 = Low priority,
                                        2 = System management interrupt,
                                        4 = Non maskable interrupt,
                                        5 = INIT,
                                        7 = External */
                     dest_mode : 1, /* Affects how the destination field is read, 0 is physical mode, 1 is logical.
                                       If the Destination Mode of this entry is Physical Mode, bits 56-59 contain an APIC ID. */
                     will_be_sent : 1, /* Set if this interrupt is going to be sent, but the APIC is busy. Read only. */
                     polarity : 1, /* 0 = High is active, 1 = Low is active. */
                     is_received : 1, /* Used for level triggered interrupts only to show if a local APIC has received the interrupt (= 1),
                                         or has sent an EOI (= 0). Read only. */
                     trigger_mode : 1, /* 0 = Edge sensitive, 1 = Level sensitive. */
                     ignore_interrupt : 1, /* Stops the interrupt from reaching the processor if set.  */
                     : 39,
                     destination : 8; /* If the destination mode bit was clear,
                                         then the lower 4 bits contain the bit APIC ID to sent the interrupt to.
                                         If the bit was set, the upper 4 bits also contain a set of processors. */
        };
        struct ATTR_PACKED
        {
            uint32_t reg1;
            uint32_t reg2;
        } _data;
    };
};

#define IO_APIC_REG_ID 0x0
#define IO_APIC_REG_VERSION 0x1
#define IO_APIC_REG_PRIORITY 0x2
#define IO_APIC_REG_REDIRECTION_ENTRIES_BASE 0x10

#define LAPIC_REG_ID 0x20
#define LAPIC_REG_VERSION 0x30
#define LAPIC_REG_TASK_PRIORITY 0x80
#define LAPIC_REG_ARBITRATION_PRIORITY 0x90
#define LAPIC_REG_PROCESSOR_PRIORITY 0xa0
#define LAPIC_REG_EOI 0xb0
#define LAPIC_REG_REMOTE_READ 0xc0
#define LAPIC_REG_LOGICAL_DESTINATION 0xd0
#define LAPIC_REG_DESTINATION_FORMAT 0xe0
#define LAPIC_REG_SPURIOUS_INTERRUPT_VECTOR 0xf0
#define LAPIC_REG_IN_SERVICE_BASE 0x100
#define LAPIC_REG_TRIGGER_MODE_BASE 0x180
#define LAPIC_REG_INTERRUPT_REQUEST_BASE 0x200
#define LAPIC_REG_ERROR_STATUS 0x280
#define LAPIC_REG_LVT_CORRECTED_MACHINE_CHECK_INTERRUPT 0x2f0
#define LAPIC_REG_INTERRUPT_COMMAND_BASE 0x300
#define LAPIC_REG_LVT_TIMER 0x320
#define LAPIC_REG_LVT_THERMAL_SENSOR 0x330
#define LAPIC_REG_LVT_PERFORMANCE_COUNTER 0x340
#define LAPIC_REG_LVT_LINT0 0x350
#define LAPIC_REG_LVT_LINT1 0x360
#define LAPIC_REG_LVT_ERROR 0x370
#define LAPIC_REG_INITIAL_COUNT 0x380
#define LAPIC_REG_CURRENT_COUNT 0x390
#define LAPIC_REG_DIVIDE_CONFIGURATION 0x3e0

void init_apic(void);
