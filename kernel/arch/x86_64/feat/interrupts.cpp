#include "kernel/arch/x86_64/feat/interrupts.h"
#include "kernel/arch/x86_64/feat/asm.h"
#include "kernel/arch/x86_64/feat/pic_8259.h"
#include "kernel/arch/x86_64/feat/ps2_keyboard.h"
#include "kernel/sys/panic.h"
#include "kernel/sys/logger.h"

#include <cstdio>

#define STACKFRAME_MAX 8

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

    struct symtab_entry_t
    {
        char* name;
        uintptr_t addr;
    };

    extern "C" symtab_entry_t _kernel_symbol_table[];

    char* trace_address(uintptr_t* off, uintptr_t addr)
    {
        for(uintptr_t i = 0;; i++)
        {
            if(_kernel_symbol_table[i].addr >= addr)
            {
                *off = addr - _kernel_symbol_table[i - 1].addr;

                return _kernel_symbol_table[i - 1].name;
            }
        }

        return nullptr;
    }

    static void keyboard_handler(x86_64::interrupt_stack_frame* frame)
    {
        x86_64::keyboard_scancode scancode = x86_64::kbps2_get_scancode();

        puts("ps/2 keyboard");
        printf("scancode=0x%x; press_type=%s; ascii='%c'\n\n", scancode, (bool)kbps2_get_key_state(scancode) ? "released" : "down", kbps2_scancode_to_ascii(scancode));

        x86_64::pic8259_master_eoi();
    }

    void print_stack_trace(stack_frame* frame, size_t size)
    {
        stack_frame* current = frame;
        int i = 0;

        log_info("trace", "Start of call trace:");

        while(current != nullptr && i++ < size)
        {
            uintptr_t offset = 0;
            char* name = trace_address(&offset, current->rip);

            if(name)
            {
                log_info("trace", "  [0x%x] %s+0x%x", current->rip, name, offset);
            }
            else
            {
                log_info("trace", "  [0x%x]", current->rip);
            }

            current = current->rbp;
        }

        if(i == 0)
        {
            log_info("trace", "  (empty)");
        }
    }

    void dump_stackframe(void* addr)
    {
        if(addr == nullptr)
        {
            addr = (void*)get_bp();
        }

        print_stack_trace((stack_frame*)addr, STACKFRAME_MAX);

        putchar('\n');
    }

    void dump_interrupt_info(interrupt_stack_frame* frame)
    {
        if(frame->int_num < 32)
        {
            printf("CPU exception! %s(0x%x) error_code=0x%x\n", exception_names[frame->int_num], frame->int_num, frame->err);

            if(frame->int_num == INT_PAGE_FAULT)
            {
                printf("caused by virtual address 0x%x\n", get_cr2());
            }

            printf("\nR8=0x%x R9=0x%x R10=0x%x R11=0x%x R12=0x%x R13=0x%x R14=0x%x R15=0x%x,\n"
                   "RAX=0x%x RBX=0x%x RCX=0x%x RDX=0x%x RSI=0x%x RDI=0x%x RBP=0x%x\n"
                   "RIP=0x%x CS=0x%x RFLAGS=0x%x RSP=0x%x SS=0x%x\n"
                   "CR0=0x%x CR2=0x%x CR3=0x%x CR4=0x%x\n\n",
                   frame->r8, frame->r9, frame->r10, frame->r11, frame->r12, frame->r13, frame->r14, frame->r15,
                   frame->rax, frame->rbx, frame->rcx, frame->rdx, frame->rsi, frame->rdi, frame->rbp,
                   frame->rip, frame->cs, frame->rflags, frame->rsp, frame->ss,
                   get_cr0(), get_cr2(), get_cr3(), get_cr4());

            putchar('\n');

            kpanic(exception_names[frame->int_num], (void*)frame->rbp);
        }
    }

    extern "C" void interrupt_handler(x86_64::interrupt_stack_frame* frame)
    {
        if(frame->int_num == INT_KEYBOARD)
        {
            keyboard_handler(frame);
        }
        else
        {
            dump_interrupt_info(frame);
        }
    }
}
