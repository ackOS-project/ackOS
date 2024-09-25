#pragma once

#include <stdint.h>

// This file should have constants common among supported architectures

#ifdef ARCH_COMMON_INC
    #include ARCH_COMMON_INC
#else
// These include:
    typedef uintptr_t phys_addr_t;
    typedef void* virt_addr_t;

    #define PAGE_SIZE 0
    #define INVALID_PHYS_ADDR (phys_addr_t)0

    struct vmm_context;
#endif