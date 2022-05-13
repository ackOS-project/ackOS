#include "kernel/arch/x86_64/feat/asm.h"

namespace x86_64
{
    void interrupts_enable()
    {
        asm volatile("sti");
    }

    void interrupts_disable()
    {
        asm volatile("cli");
    }

    void halt()
    {
        asm volatile("hlt");
    }

    uint64_t get_bp()
    {
        uint64_t bp;
        asm volatile("movq %%rbp, %0" : "=g"(bp) :: "memory");

        return bp;
    }

    uint64_t get_cr0()
    {
        uint64_t cr;
        asm volatile("mov %%cr0, %0" : "=r"(cr));

        return cr;
    }

    uint64_t get_cr1()
    {
        uint64_t cr;
        asm volatile("mov %%cr1, %0" : "=r"(cr));

        return cr;
    }

    uint64_t get_cr2()
    {
        uint64_t cr;
        asm volatile("mov %%cr2, %0" : "=r"(cr));

        return cr;
    }

    uint64_t get_cr3()
    {
        uint64_t cr;
        asm volatile("mov %%cr3, %0" : "=r"(cr));

        return cr;
    }

    uint64_t get_cr4()
    {
        uint64_t cr;
        asm volatile("mov %%cr4, %0" : "=r"(cr));

        return cr;
    }

    void set_cr0(uint64_t value)
    {
        asm volatile("mov %0, %%cr0" :: "r"(value));
    }

    void set_cr1(uint64_t value)
    {
        asm volatile("mov %0, %%cr1" :: "r"(value));
    }

    void set_cr2(uint64_t value)
    {
        asm volatile("mov %0, %%cr2" :: "r"(value));
    }

    void set_cr3(uint64_t value)
    {
        asm volatile("mov %0, %%cr3" :: "r"(value));
    }

    void set_cr4(uint64_t value)
    {
        asm volatile("mov %0, %%cr4" :: "r"(value));
    }
}
