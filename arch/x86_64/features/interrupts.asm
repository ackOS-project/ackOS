bits 64
section .text

extern interrupt_handler

%macro pusha 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popa 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

interrupt_common_handler:
    cld
    pusha

    mov rdi, rsp
    call interrupt_handler
    mov rsp, rax
    popa

    add rsp, 16

    iretq

%macro DEF_INTERRUPT 1
int%1:
    push 0
    push %1
    jmp interrupt_common_handler
%endmacro

%macro DEF_INTERRUPT_ERR 1
int%1:
    push %1
    jmp interrupt_common_handler
%endmacro

DEF_INTERRUPT 0
DEF_INTERRUPT 1
DEF_INTERRUPT 2
DEF_INTERRUPT 3
DEF_INTERRUPT 4
DEF_INTERRUPT 5
DEF_INTERRUPT 6
DEF_INTERRUPT 7
DEF_INTERRUPT_ERR 8
DEF_INTERRUPT 9
DEF_INTERRUPT_ERR 10
DEF_INTERRUPT_ERR 11
DEF_INTERRUPT_ERR 12
DEF_INTERRUPT_ERR 13
DEF_INTERRUPT_ERR 14
DEF_INTERRUPT 15
DEF_INTERRUPT 16
DEF_INTERRUPT_ERR 17
DEF_INTERRUPT 18
DEF_INTERRUPT 19
DEF_INTERRUPT 20
DEF_INTERRUPT 21
DEF_INTERRUPT 22
DEF_INTERRUPT 23
DEF_INTERRUPT 24
DEF_INTERRUPT 25
DEF_INTERRUPT 26
DEF_INTERRUPT 27
DEF_INTERRUPT 28
DEF_INTERRUPT 29
DEF_INTERRUPT_ERR 30
DEF_INTERRUPT 31
DEF_INTERRUPT 32
DEF_INTERRUPT 33
DEF_INTERRUPT 34
DEF_INTERRUPT 35
DEF_INTERRUPT 36
DEF_INTERRUPT 37
DEF_INTERRUPT 38
DEF_INTERRUPT 39
DEF_INTERRUPT 40
DEF_INTERRUPT 41
DEF_INTERRUPT 42
DEF_INTERRUPT 43
DEF_INTERRUPT 44
DEF_INTERRUPT 45
DEF_INTERRUPT 46
DEF_INTERRUPT 47

global _interrupt_vt
_interrupt_vt:
    %macro INT_NAME 1
        dq int%1
    %endmacro

    %assign i 0
    %rep 47
    INT_NAME i
    %assign i i+1
    %endrep
