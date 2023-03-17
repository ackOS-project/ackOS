#pragma once

#include <stddef.h>
#include <stdint.h>

#include <liback/util.h>

struct ATTR_PACKED idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist : 3, /* Interrupt Stack Table */
            zero0 : 5;
    union {
        struct {
            uint8_t type : 5,
                    privilege_level : 2,
                    present : 1;
        };
        uint8_t types_attr;
    };
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero1;
};

struct ATTR_PACKED idt_descriptor
{
    uint16_t limit;
    uintptr_t offset;
};

struct ATTR_PACKED int_frame
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t int_num;
    uint64_t err;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

void init_idt(void);
