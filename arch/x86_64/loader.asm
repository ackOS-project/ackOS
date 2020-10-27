extern kmain
global _start

section .text
; We start in 32 bit mode and move into 64-bit mode
bits 32
_start:
    ; Initalize before loading the kernel
    mov esp, stack_top

    mov dword [0xb8000], 0x2f4b2f4f
    hlt

    call kmain

    hlt

; Errors
loader_print_error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    mov byte  [0xb800c], " " ; Remove any extra text such as QEMU status 
    ret

loader_print_unsupported_err:
    mov al, "0"
    call loader_print_error
    hlt

section .bss
align 4096
; Paging tables
p4_table:
    resb 4096

p3_table:
    resb 4096

p2_table:
    resb 4096

stack_bottom:
    resb 64
stack_top: