extern long_mode_init
global _start

section .text
bits 32
_start:
    ; Move the multiboot information into variables that we can access in long mode
    mov dword[multiboot2_header], eax
    mov dword[multiboot2_magic], ebx

    ; Initalize before loading the kernel
    mov esp, stack_top

    ; Do all the checks
    call multiboot_check
    call cpuid_check_long_mode

    call set_up_page_tables
    call enable_paging

    ; Load gdt
    lgdt [_gdt_descriptor]
    jmp _gdt.code:long_mode_init

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

section .data
_gdt:
    .zero: equ $ - _gdt
        dq 0 ; zero entry

    .code: equ $ - _gdt
        dw 0x0000       ; limit
        dw 0x0000       ; base (low 16 bits)
        db 0x00         ; base (mid 8 bits)
        db 10011010b    ; access
        db 00100000b    ; granularity
        db 0x00         ; base (high 8 bits)

    .data: equ $ - _gdt
        dw 0            ; limit
        dw 0            ; base (low 16 bits)
        db 0            ; base (mid 8 bits)
        db 10010010b    ; access
        db 00000000b    ; granularity
        db 0            ; base (high 8 bits)

    .task: equ $ - _gdt
        dq 0
        dq 0

        dw 0

_gdt_descriptor:
    dw $ - _gdt - 1
    dq _gdt

global multiboot2_header
global multiboot2_magic
multiboot2_header: dq 0
multiboot2_magic: dq 0
