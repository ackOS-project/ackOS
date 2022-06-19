section .bss
align 16

global _kernel_stack_top
global _kernel_stack_bottom

_kernel_stack_bottom:
    resb 1048576 * 8 ; this will give us 8MB of space
_kernel_stack_top: