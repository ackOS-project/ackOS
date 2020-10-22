extern kmain
global _start

_start:
    ; Initalize before loading the kernel
    call kmain
    hlt

section .text
stack_init:
    mov esp, stack_end ; Set the stack pointer

section .bss
stack_start:
    resb 4096
stack_end: