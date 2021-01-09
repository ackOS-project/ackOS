global long_mode_init

section .text
; We are now in 64-bit long mode!
bits 64
long_mode_init:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern x86_64_init

    extern multiboot2_header
    extern multiboot2_magic

    mov edi, dword multiboot2_header
    mov esi, dword[multiboot2_magic]
    call x86_64_init

    hlt
