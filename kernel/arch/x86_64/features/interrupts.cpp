#include "kernel/arch/x86_64/features/interrupts.h"
#include "kernel/arch/x86_64/features/instructions.h"
#include "kernel/arch/x86_64/features/pic_8259.h"
#include "kernel/arch/x86_64/features/ps2_keyboard.h"
#include "kernel/panic.h"

#include <cstdio>

namespace x86_64
{
    static const char* exception_names[] =
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

    static void keyboard_handler(x86_64::interrupt_stack_frame* frame)
    {
        x86_64::keyboard_scancode scancode = x86_64::kbps2_get_scancode();

        puts("ps/2 keyboard");
        printf("scancode=0x%x; press_type=%s; ascii='%c'\n\n", scancode, (bool)kbps2_get_key_state(scancode) ? "released" : "down", kbps2_scancode_to_ascii(scancode));

        x86_64::pic8259_master_eoi();
    }

    void print_stack_trace(stack_frame* frame)
    {
        stack_frame* current = frame;

        while(current != nullptr)
        {
            printf("0x%x\n", current->rip);

            current = current->rbp;
        }
    }

    void dump_stack_frame(interrupt_stack_frame* frame)
    {
        printf("R8=0x%x R9=0x%x R10=0x%x R11=0x%x R12=0x%x R13=0x%x R14=0x%x R15=0x%x,\n", frame->r8, frame->r9, frame->r10, frame->r11, frame->r12, frame->r13, frame->r14, frame->r15);
        printf("RAX=0x%x RBX=0x%x RCX=0x%x RDX=0x%x RSI=0x%x RDI=0x%x RBP=0x%x\n", frame->rax, frame->rbx, frame->rcx, frame->rdx, frame->rsi, frame->rdi, frame->rbp);
        printf("RIP=0x%x CS=0x%x RFLAGS=0x%x RSP=0x%x SS=0x%x\n", frame->rip, frame->cs, frame->rflags, frame->rsp, frame->ss);
        //printf("CR0=0x%x CR1=0x%x CR2=0x%x CR3=0x%x CR4=0x%x\n\n", get_cr0(), get_cr1(), get_cr2(), get_cr3(), get_cr4());

        puts("\nStack Trace: ");
        print_stack_trace(reinterpret_cast<stack_frame*>(frame->rbp));

        putchar('\n');
    }

    extern "C" void interrupt_handler(x86_64::interrupt_stack_frame* frame)
    {
        if(frame->int_num == 0x21)
        {
            keyboard_handler(frame);
        }
        else
        {
            printf("unhandled interrupt! int_num=0x%x; error=0x%x\n\n", frame->int_num, frame->err);
            dump_stack_frame(frame);

            kpanic(exception_names[frame->int_num]);
        }
    }
}
