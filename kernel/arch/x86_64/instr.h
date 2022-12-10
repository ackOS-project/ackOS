#pragma once

#include <stdint.h>
#include <stddef.h>

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

    __asm__ volatile("movq %%rbp, %0" : "=g"(bp) :: "memory");

    return bp;
}

inline static uint64_t reg_get_cr0(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr0, %0" : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr2(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr2, %0" : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr3(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr3, %0" : "=r"(cr));

    return cr;
}

inline static uint64_t reg_get_cr4(void)
{
    uint64_t cr;

    __asm__ volatile("movq %%cr4, %0" : "=r"(cr));

    return cr;
}

inline static void reg_set_cr0(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr0" :: "r"(value));
}


inline static void reg_set_cr2(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr2" :: "r"(value));
}


inline static void reg_set_cr3(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr3" :: "r"(value));
}


inline static void reg_set_cr4(uint64_t value)
{
    __asm__ volatile("mov %0, %%cr4" :: "r"(value));
}

