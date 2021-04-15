extern irq1_handler
extern double_fault_handler
extern protection_fault_handler

%macro pusha 0
    cld
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

global interrupt_irq1
interrupt_irq1:
    pusha
    mov rdi, rsp
    call irq1_handler
    popa
    iretq

global interrupt_double_fault
interrupt_double_fault:
    pusha
    mov rdi, rsp
    call double_fault_handler
    popa
    iretq

global interrupt_protection_fault
interrupt_protection_fault:
    pusha
    mov rdi, rsp
    call protection_fault_handler
    popa
    iretq
