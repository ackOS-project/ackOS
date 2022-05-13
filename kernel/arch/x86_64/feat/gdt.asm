bits 64

global gdt64_load
global gdt64_32bit_trampoline

gdt64_load:
    lgdt [rdi]

    push rbp
    mov rbp, rsp

    push rsi
    push rbp
    pushfq

    push rdx
    push .trampoline

    iretq

.trampoline:
    pop rbp

    ; flush the registers
    mov ss, rsi
    mov gs, rsi
    mov fs, rsi
    mov ds, rsi
    mov es, rsi

    ret

global tss64_flush
tss64_flush:
    mov ax, 0x28

    ltr ax

    ret
