#pragma once

#include "kernel/arch/x86_64/gdt.h"

#include <stddef.h>
#include <stdint.h>

inline static void halt(void)
{
    __asm__ volatile("hlt");
}

inline static void int_enable(void)
{
    __asm__ volatile("sti");
}

inline static void int_disable(void)
{
    __asm__ volatile("cli");
}

static inline uint64_t reg_get_bp(void)
{
    uint64_t bp;

    __asm__ volatile("movq %%rbp, %0"
                     : "=g"(bp)::"memory");

    return bp;
}

inline static uint64_t reg_get_cr0(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr0, %0"
                     : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr2(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr2, %0"
                     : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr3(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr3, %0"
                     : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr4(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr4, %0"
                     : "=r"(cr));

    return cr;
}

inline static void reg_set_cr0(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr0" ::"r"(value));
}

inline static void reg_set_cr2(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr2" ::"r"(value));
}

inline static void reg_set_cr3(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr3" ::"r"(value));
}

inline static void reg_set_cr4(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr4" ::"r"(value));
}

inline static uint16_t reg_get_cs(void)
{
    uint16_t reg;

    __asm__ volatile("mov %%cs, %0"
                     : "=r"(reg));

    return reg;
}

inline static uint16_t reg_get_ds(void)
{
    uint16_t reg;

    __asm__ volatile("mov %%ds, %0"
                     : "=r"(reg));

    return reg;
}

inline static struct gdt_descriptor reg_get_gdtr(void)
{
    struct gdt_descriptor desc;

    __asm__ volatile("sgdt %0"
                     : "=m"(desc));

    return desc;
}

inline static uint8_t inb(uint16_t port)
{
    uint8_t r;

    __asm__ __volatile__("inb %1, %0"
                         : "=a"(r)
                         : "dN"(port));

    return r;
}

inline static void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %1, %0"
                         :
                         : "dN"(port), "a"(data));
}

inline static void outw(uint16_t port, uint16_t data)
{
    __asm__ volatile("outw %%ax, %%dx"
                 :
                 : "d"(port), "a"(data));
}

inline static void io_wait(void)
{
    __asm__ volatile("jmp 1f\n\t"
                 "1:jmp 2f\n\t"
                 "2:");
}
