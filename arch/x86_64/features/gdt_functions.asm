bits 64

global gdt_load

gdt_load:
    lgdt [rdi]

    ; flush the registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    pop rdi

    mov rax, 0x08
    push rax
    push rdi

    retfq
