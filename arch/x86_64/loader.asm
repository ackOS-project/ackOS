extern x86_64_init
global _start

section .text
_start:
    ; Initalize before loading the kernel
    mov esp, stack_top

    ; Do all the checks
    call multiboot_check
    call cpuid_check_long_mode

    call x86_64_init
    cmp eax, 0
    jg .loader_error

    hlt
.loader_error:
    mov al, "9" ; temporarily
    jmp loader_print_error

; thanks to https://os.phil-opp.com/entering-longmode/
cpuid_check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "2"
    jmp loader_print_error

multiboot_check:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "0"
    jmp loader_print_error

global enable_a20
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Errors
loader_print_error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    mov byte  [0xb800c], " " ; Remove any extra text such as QEMU status
    hlt
    ret

loader_print_unknown_err:
    mov al, "9"
    call loader_print_error

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