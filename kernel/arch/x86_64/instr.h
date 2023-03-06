#pragma once

#include "kernel/arch/x86_64/gdt.h"

#include <stddef.h>
#include <stdint.h>

static inline void halt(void)
{
    __asm__ volatile("hlt");
}

static inline void int_enable(void)
{
    __asm__ volatile("sti");
}

static inline void int_disable(void)
{
    __asm__ volatile("cli");
}

static inline void invalidate_page(void* virt)
{
    __asm__ volatile("invlpg (%0)" :: "r"(virt) : "memory");
}

static inline uint64_t reg_get_rax(void)
{
    uint64_t rax;

    __asm__ volatile("movq %%rax, %0"
                     : "=g"(rax)::"memory");

    return rax;
}

static inline uint64_t reg_get_rbx(void)
{
    uint64_t rbx;

    __asm__ volatile("movq %%rbx, %0"
                     : "=g"(rbx)::"memory");

    return rbx;
}

static inline uint64_t reg_get_rcx(void)
{
    uint64_t rcx;

    __asm__ volatile("movq %%rcx, %0"
                     : "=g"(rcx)::"memory");

    return rcx;
}

static inline uint64_t reg_get_rdx(void)
{
    uint64_t rdx;

    __asm__ volatile("movq %%rdx, %0"
                     : "=g"(rdx)::"memory");

    return rdx;
}

static inline uint64_t reg_get_rsi(void)
{
    uint64_t rsi;

    __asm__ volatile("movq %%rsi, %0"
                     : "=g"(rsi)::"memory");

    return rsi;
}

static inline uint64_t reg_get_rdi(void)
{
    uint64_t rdi;

    __asm__ volatile("movq %%rdi, %0"
                     : "=g"(rdi)::"memory");

    return rdi;
}

static inline uint64_t reg_get_r8(void)
{
    uint64_t r8;

    __asm__ volatile("movq %%r8, %0"
                     : "=g"(r8)::"memory");

    return r8;
}

static inline uint64_t reg_get_r9(void)
{
    uint64_t r9;

    __asm__ volatile("movq %%r9, %0"
                     : "=g"(r9)::"memory");

    return r9;
}

static inline uint64_t reg_get_r10(void)
{
    uint64_t r10;

    __asm__ volatile("movq %%r10, %0"
                     : "=g"(r10)::"memory");

    return r10;
}

static inline uint64_t reg_get_r11(void)
{
    uint64_t r11;

    __asm__ volatile("movq %%r11, %0"
                     : "=g"(r11)::"memory");

    return r11;
}

static inline uint64_t reg_get_r12(void)
{
    uint64_t r12;

    __asm__ volatile("movq %%r12, %0"
                     : "=g"(r12)::"memory");

    return r12;
}


static inline uint64_t reg_get_r13(void)
{
    uint64_t r13;

    __asm__ volatile("movq %%r13, %0"
                     : "=g"(r13)::"memory");

    return r13;
}

static inline uint64_t reg_get_r14(void)
{
    uint64_t r14;

    __asm__ volatile("movq %%r14, %0"
                     : "=g"(r14)::"memory");

    return r14;
}

static inline uint64_t reg_get_r15(void)
{
    uint64_t r15;

    __asm__ volatile("movq %%r15, %0"
                     : "=g"(r15)::"memory");

    return r15;
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

void print_registers(void);
