.section .text

.globl gdt_load

/*FFFFFFFF80006BF0*/
/*
desc: rdi, code: si, data: dx
*/
gdt_load:
    /* ensure the upper bits (63..16) of the segment selector arguments are clear. */
    movzwq %si, %rsi
    movzwq %dx, %rdx

    /* save stack */
    pushq %rbp
    movq %rsp, %rbp

    lgdt (%rdi)

    pushq %rdx /* push stack segment (same as data segment) */
    pushq %rbp /* push stack pointer */
    pushfq /* push rflags register */
    pushq %rsi /* push code segment */
    pushq $trampoline /* ireq will pop 'trampoline' into the instruction pointer and start executing */

    iretq

trampoline:
    movq %rbp, %rsp
    popq %rbp

    /* flush the segment registers with the new data segment */
    movw %dx, %ss
    movw %dx, %gs
    movw %dx, %fs
    movw %dx, %ds
    movw %dx, %es

    popq %rdi

    //xchgw %bx, %bx

    jmpq *%rdi

    retq
    
