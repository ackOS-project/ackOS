bits 64

global gdt64_load

gdt64_load:
    lgdt [rdi]

    push rbp
    mov rbp, rsp

    push qword 0x10
    push rbp
    pushfq

    push qword 0x8
    push .trampoline ; kernel code selector

    iretq

.trampoline:
    pop rbp

    ; flush the registers
    mov ax, 0x10 ; kernel data selector

    mov ss, ax
    mov gs, ax
    mov fs, ax
    mov ds, ax
    mov es, ax

    ret

global tss64_flush
tss64_flush:
    mov ax, 0x28

    ltr ax

    ret
