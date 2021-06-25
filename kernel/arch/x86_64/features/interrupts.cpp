#include "kernel/arch/x86_64/features/interrupts.h"
#include "kernel/arch/x86_64/features/pic_8259.h"
#include "kernel/arch/x86_64/features/ps2_keyboard.h"
#include "kernel/panic.h"

#include <cstdio>

const char* exception_names[] =
{
    "division by zero error",
    "debug",
    "non-maskable interrupt",
    "breakpoint",
    "overflow",
    "bound range exceeded",
    "invalid opcode",
    "device not available",
    "double fault",
    "coprocessor segment overrun (deprecated)",
    "invalid TSS",
    "segment not present",
    "stack segment fault",
    "general protection fault",
    "page fault",
    "(reserved)",
    "x87 FPU error",
    "alignment check",
    "machine check",
    "SIMD floating point error",
    "virtualisation error",
    "(reserved)",
    "control protection exception",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)",
    "(reserved)"
};

void* keyboard_handler(x86_64::interrupt_stack_frame* frame)
{
    x86_64::keyboard_scancode scancode = x86_64::kbps2_get_scancode();

    puts("ps/2 keyboard");
    printf("scancode=0x%x; press_type=%s; ascii='%c'\n\n", scancode, (bool)kbps2_get_key_state(scancode) ? "released" : "down", kbps2_scancode_to_ascii(scancode));

    x86_64::pic8259_master_eoi();

    return (void*)frame;
}

void double_fault_handler(x86_64::interrupt_stack_frame* frame)
{
    kpanic("double fault");
}

void gp_fault_handler(x86_64::interrupt_stack_frame* frame)
{
    kpanic("general protection fault");
}

void page_fault_handler(x86_64::interrupt_stack_frame* frame)
{
    kpanic("page fault");
}

extern "C" void interrupt_handler(x86_64::interrupt_stack_frame* frame)
{
    printf("int_num=0x%x; error=0x%x\n", frame->int_num, frame->err);

    if(frame->int_num == 0x21)
    {
        keyboard_handler(frame);
    }
    else if(frame->int_num == 0xD)
    {
        gp_fault_handler(frame);
    }
    else if(frame->int_num == 0x8)
    {
        double_fault_handler(frame);
    }
    else if(frame->int_num == 0xE)
    {
        page_fault_handler(frame);
    }
}
