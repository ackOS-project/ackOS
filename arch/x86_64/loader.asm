extern long_mode_init
global _start

section .text
bits 32
_start:
    ; Initalize before loading the kernel
    mov esp, stack_top

    ; Do all the checks
    call multiboot_check
    call cpuid_check_long_mode

    call set_up_page_tables
    call enable_paging

    ; Load gdt
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_init

    ; Check if it's an error. more than 0
    ; cmp eax, 0
    ; jg .loader_error

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
    cmp eax, 0x36D76289
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

global set_up_page_tables
set_up_page_tables:
    ; map first P4 entry to P3 table
    mov eax, p3_table
    or eax, 0b11 ; present + writable
    mov [p4_table], eax

    ; map first P3 entry to P2 table
    mov eax, p2_table
    or eax, 0b11 ; present + writable
    mov [p3_table], eax

    mov ecx, 0


.p2_table_map:
    ; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 0x200000  ; 2MiB
    mul ecx            ; start address of ecx-th page
    or eax, 0b10000011 ; present + writable + huge
    mov [p2_table + ecx * 8], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
    jne .p2_table_map  ; else map the next entry

    ret

global enable_paging
enable_paging:
    mov eax, p4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

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

section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64