bits 64

global gdt64_load

gdt64_load:
    lgdt [rdi]

    push rbp
    mov rbp, rsp

    push rax
    push rbp
    pushfq

    push rdx
    push .trampoline

    iretq

.trampoline:
    pop rbp

    ; flush the registers
    mov ss, rax
    mov gs, rax
    mov fs, rax
    mov ds, rax
    mov es, rax

    ret

global tss64_flush
tss64_flush:
    mov ax, 0x28

    ltr ax

    ret
