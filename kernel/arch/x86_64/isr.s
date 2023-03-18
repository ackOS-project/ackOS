.macro pusha
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
.endm

.macro popa
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
.endm

.section .text

.extern interrupt_handler

.align 4
isr_handler:
    cld
    pusha

    movq %rsp, %rdi /* load register frame pointer into the first argument (rdi) */
    callq interrupt_handler
    movq %rax, %rsp
    popa

    add $16, %rsp /* pop error code and int code (8 bytes each) */

    iretq

.macro DEF_INT_ISR num
    isr\num:
        pushq $0 /* error code: none */
        pushq $\num

        jmp isr_handler
.endm

.macro DEF_ERR_ISR num
    isr\num:
        /* error code already pushed onto the stack by the CPU */
        pushq $\num
        jmp isr_handler
.endm

DEF_INT_ISR 0
DEF_INT_ISR 1
DEF_INT_ISR 2
DEF_INT_ISR 3
DEF_INT_ISR 4
DEF_INT_ISR 5
DEF_INT_ISR 6
DEF_INT_ISR 7
DEF_ERR_ISR 8
DEF_INT_ISR 9
DEF_ERR_ISR 10
DEF_ERR_ISR 11
DEF_ERR_ISR 12
DEF_ERR_ISR 13
DEF_ERR_ISR 14
DEF_INT_ISR 15
DEF_INT_ISR 16
DEF_ERR_ISR 17
DEF_INT_ISR 18
DEF_INT_ISR 19
DEF_INT_ISR 20
DEF_INT_ISR 21
DEF_INT_ISR 22
DEF_INT_ISR 23
DEF_INT_ISR 24
DEF_INT_ISR 25
DEF_INT_ISR 26
DEF_INT_ISR 27
DEF_INT_ISR 28
DEF_INT_ISR 29
DEF_ERR_ISR 30

.altmacro

/* define remaining ISRs */
.set i, 31
.rept 225
    DEF_INT_ISR %i
    .set i, i + 1
.endr

.section .data

/* fill the vector list */

.macro DEF_VECTOR_LABEL num
    .quad isr\num
.endm

.globl idt_vect_list
idt_vect_list:
.set i, 0
.rept 256
    DEF_VECTOR_LABEL %i
    .set i, i + 1
.endr

