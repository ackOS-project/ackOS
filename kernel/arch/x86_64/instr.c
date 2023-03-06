#include "kernel/arch/x86_64/instr.h"

#include "kernel/lib/log.h"


void print_registers()
{
    kprintf(KERN_DEBUG "Registers:\n"
                       "    rax:      %#-20lx rbx:      %#lx\n"
                       "    rcx:      %#-20lx rdx:      %#lx\n"
                       "    rsi:      %#-20lx rdi:      %#lx\n"
                       "    r8:       %#-20lx r9:       %#lx\n"
                       "    r10:      %#-20lx r11:      %#lx\n"
                       "    r12:      %#-20lx r13:      %#lx\n"
                       "    r14:      %#-20lx r15:      %#lx\n"
                       "    cr0:      %#-20lx cr2:      %#lx\n"
                       "    cr3:      %#-20lx cr4:      %#lx\n"
                       "    rbp:      %#lx\n",
                       reg_get_rax(),
                       reg_get_rbx(),
                       reg_get_rcx(),
                       reg_get_rdx(),
                       reg_get_rsi(),
                       reg_get_rdi(),
                       reg_get_r8(),
                       reg_get_r9(),
                       reg_get_r10(),
                       reg_get_r11(),
                       reg_get_r12(),
                       reg_get_r13(),
                       reg_get_r14(),
                       reg_get_r15(),
                       reg_get_cr0(),
                       reg_get_cr2(),
                       reg_get_cr3(),
                       reg_get_cr4(),
                       reg_get_bp());
}
