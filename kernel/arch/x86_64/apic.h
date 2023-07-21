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

#define IO_APIC_REG_ID 0
#define IO_APIC_REG_VERSION 1
#define IO_APIC_REG_PRIORITY 2
#define IO_APIC_REG_REDIRECTION_ENTRIES_BASE 16

void init_apic(void);
