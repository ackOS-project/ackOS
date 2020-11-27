extern _idt
extern irq1_handler

global interrupt_irq1
interrupt_irq1:
    call irq1_handler
    iretq

global idt_load
idt_load:
    lidt [rdi]
    sti
    ret
