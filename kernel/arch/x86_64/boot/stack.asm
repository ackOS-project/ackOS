section .bss
align 16

global _kernel_stack_top
global _kernel_stack_bottom

_kernel_stack_bottom:
    resb 1024 * 16 ; this will give us 16KB of space
_kernel_stack_top: