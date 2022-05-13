#pragma once

#include <liback/utils/macros.h>
#include <cstddef>
#include <cstdint>

namespace x86_64
{
    struct interrupt_stack_frame
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
    }
    ATTRIBUTE_PACKED;

    struct stack_frame
    {
        stack_frame* rbp;
        uint64_t rip;
    }
    ATTRIBUTE_PACKED;

    enum interrupt_code
    {
        INT_DIV_BY_ZERO,
        INT_DEBUG,
        INT_NON_MASKABLE_INTERRUPT,
        INT_BREAKPOINT,
        INT_OVERFLOW,
        INT_BOUND_RANGE_EXCEEDED,
        INT_INVALID_OPCODE,
        INT_DEV_NOT_AVALIABLE,
        INT_DOUBLE_FAULT,
        INT_COPROCESSOR_SEG_OVERRUN,
        INT_INVALID_TSS,
        INT_SEGMENT_NOT_PRESENT,
        INT_STACK_SEG_FAULT,
        INT_GENERAL_PROTECTION_FAULT,
        INT_PAGE_FAULT,
        INT_x87_FPU_ERROR = 16,
        INT_ALIGNMENT_CHECK,
        INT_MACHINE_CHECK,
        INT_SIMD_FP_ERROR,
        INT_VIRTUALISATION_ERROR,
        INT_CONTROL_PROTECTION_EXCEPTION,
        INT_PIT = 32,
        INT_KEYBOARD,
        INT_CASCADE,
        INT_COM2,
        INT_COM1,
        INT_LPT2,
        INT_FLOPPY_DISK,
        INT_LPT1,
        INT_CMOS,
        INT_MOUSE = 44,
        INT_FPU,
        INT_PRIMARY_ATA,
        INT_SECONDARY_ATA
    };

    void dump_stackframe(void* addr);
}
