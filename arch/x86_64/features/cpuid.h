#pragma once

#include <stdint.h>
#include <stddef.h>

namespace x86_64::cpuid
{
    enum : uint32_t
    {
        GET_VENDOR_STRING,
        GET_FEATURES,
        GET_TLB,
        GET_SERIAL,

        INTEL_EXTENDED = 0x80000000,
        INTEL_FEATURES,
        INTEL_BRAND_STRING,
        INTEL_BRAND_STRING_MORE,
        INTEL_BRAND_STRING_END
    };

    enum : uint32_t
    {
        FEAT_ECX_SSE3         = 1 << 0, 
        FEAT_ECX_PCLMUL       = 1 << 1,
        FEAT_ECX_DTES64       = 1 << 2,
        FEAT_ECX_MONITOR      = 1 << 3,  
        FEAT_ECX_DS_CPL       = 1 << 4,  
        FEAT_ECX_VMX          = 1 << 5,  
        FEAT_ECX_SMX          = 1 << 6,  
        FEAT_ECX_EST          = 1 << 7,  
        FEAT_ECX_TM2          = 1 << 8,  
        FEAT_ECX_SSSE3        = 1 << 9,  
        FEAT_ECX_CID          = 1 << 10,
        FEAT_ECX_FMA          = 1 << 12,
        FEAT_ECX_CX16         = 1 << 13, 
        FEAT_ECX_ETPRD        = 1 << 14, 
        FEAT_ECX_PDCM         = 1 << 15, 
        FEAT_ECX_PCIDE        = 1 << 17, 
        FEAT_ECX_DCA          = 1 << 18, 
        FEAT_ECX_SSE4_1       = 1 << 19, 
        FEAT_ECX_SSE4_2       = 1 << 20, 
        FEAT_ECX_x2APIC       = 1 << 21, 
        FEAT_ECX_MOVBE        = 1 << 22, 
        FEAT_ECX_POPCNT       = 1 << 23, 
        FEAT_ECX_AES          = 1 << 25, 
        FEAT_ECX_XSAVE        = 1 << 26, 
        FEAT_ECX_OSXSAVE      = 1 << 27, 
        FEAT_ECX_AVX          = 1 << 28,
    
        FEAT_EDX_FPU          = 1 << 0,  
        FEAT_EDX_VME          = 1 << 1,  
        FEAT_EDX_DE           = 1 << 2,  
        FEAT_EDX_PSE          = 1 << 3,  
        FEAT_EDX_TSC          = 1 << 4,  
        FEAT_EDX_MSR          = 1 << 5,  
        FEAT_EDX_PAE          = 1 << 6,  
        FEAT_EDX_MCE          = 1 << 7,  
        FEAT_EDX_CX8          = 1 << 8,  
        FEAT_EDX_APIC         = 1 << 9,  
        FEAT_EDX_SEP          = 1 << 11, 
        FEAT_EDX_MTRR         = 1 << 12, 
        FEAT_EDX_PGE          = 1 << 13, 
        FEAT_EDX_MCA          = 1 << 14, 
        FEAT_EDX_CMOV         = 1 << 15, 
        FEAT_EDX_PAT          = 1 << 16, 
        FEAT_EDX_PSE36        = 1 << 17, 
        FEAT_EDX_PSN          = 1 << 18, 
        FEAT_EDX_CLF          = 1 << 19, 
        FEAT_EDX_DTES         = 1 << 21, 
        FEAT_EDX_ACPI         = 1 << 22, 
        FEAT_EDX_MMX          = 1 << 23, 
        FEAT_EDX_FXSR         = 1 << 24, 
        FEAT_EDX_SSE          = 1 << 25, 
        FEAT_EDX_SSE2         = 1 << 26, 
        FEAT_EDX_SS           = 1 << 27, 
        FEAT_EDX_HTT          = 1 << 28, 
        FEAT_EDX_TM1          = 1 << 29, 
        FEAT_EDX_IA64         = 1 << 30 /*,
        FEAT_EDX_PBE          = 1 << 31 -- Commented out because `enumerator value evaluates to -2147483648, which cannot be narrowed to type 'uint32_t'` */
    };
}
