/* CPU Vendor strings. */
#define CPUID_VENDOR_OLDAMD       "AMDisbetter!"
#define CPUID_VENDOR_AMD          "AuthenticAMD"
#define CPUID_VENDOR_INTEL        "GenuineIntel"
#define CPUID_VENDOR_VIA1         "CentaurHauls"
#define CPUID_VENDOR_VIA2         "VIA VIA VIA "
#define CPUID_VENDOR_OLDTRANSMETA "TransmetaCPU"
#define CPUID_VENDOR_TRANSMETA    "GenuineTMx86"
#define CPUID_VENDOR_CYRIX        "CyrixInstead"
#define CPUID_VENDOR_CENTAUR      "CentaurHauls"
#define CPUID_VENDOR_NEXGEN       "NexGenDriven"
#define CPUID_VENDOR_UMC          "UMC UMC UMC "
#define CPUID_VENDOR_SIS          "SiS SiS SiS "
#define CPUID_VENDOR_NSC          "Geode by NSC"
#define CPUID_VENDOR_RISE         "RiseRiseRise"
#define CPUID_VENDOR_VORTEX       "Vortex86 SoC"
 
/* for Virtual Machines. */
#define CPUID_VENDOR_VMWARE       "VMwareVMware"
#define CPUID_VENDOR_XENHVM       "XenVMMXenVMM"
#define CPUID_VENDOR_MICROSOFT_HV "Microsoft Hv"
#define CPUID_VENDOR_PARALLELS    " lrpepyh vr"

enum class cpuid_feature {
    ECX_SSE3         = 1 << 0, 
    ECX_PCLMUL       = 1 << 1,
    ECX_DTES64       = 1 << 2,
    ECX_MONITOR      = 1 << 3,  
    ECX_DS_CPL       = 1 << 4,  
    ECX_VMX          = 1 << 5,  
    ECX_SMX          = 1 << 6,  
    ECX_EST          = 1 << 7,  
    ECX_TM2          = 1 << 8,  
    ECX_SSSE3        = 1 << 9,  
    ECX_CID          = 1 << 10,
    ECX_FMA          = 1 << 12,
    ECX_CX16         = 1 << 13, 
    ECX_ETPRD        = 1 << 14, 
    ECX_PDCM         = 1 << 15, 
    ECX_PCIDE        = 1 << 17, 
    ECX_DCA          = 1 << 18, 
    ECX_SSE4_1       = 1 << 19, 
    ECX_SSE4_2       = 1 << 20, 
    ECX_x2APIC       = 1 << 21, 
    ECX_MOVBE        = 1 << 22, 
    ECX_POPCNT       = 1 << 23, 
    ECX_AES          = 1 << 25, 
    ECX_XSAVE        = 1 << 26, 
    ECX_OSXSAVE      = 1 << 27, 
    ECX_AVX          = 1 << 28,
 
    EDX_FPU          = 1 << 0,  
    EDX_VME          = 1 << 1,  
    EDX_DE           = 1 << 2,  
    EDX_PSE          = 1 << 3,  
    EDX_TSC          = 1 << 4,  
    EDX_MSR          = 1 << 5,  
    EDX_PAE          = 1 << 6,  
    EDX_MCE          = 1 << 7,  
    EDX_CX8          = 1 << 8,  
    EDX_APIC         = 1 << 9,  
    EDX_SEP          = 1 << 11, 
    EDX_MTRR         = 1 << 12, 
    EDX_PGE          = 1 << 13, 
    EDX_MCA          = 1 << 14, 
    EDX_CMOV         = 1 << 15, 
    EDX_PAT          = 1 << 16, 
    EDX_PSE36        = 1 << 17, 
    EDX_PSN          = 1 << 18, 
    EDX_CLF          = 1 << 19, 
    EDX_DTES         = 1 << 21, 
    EDX_ACPI         = 1 << 22, 
    EDX_MMX          = 1 << 23, 
    EDX_FXSR         = 1 << 24, 
    EDX_SSE          = 1 << 25, 
    EDX_SSE2         = 1 << 26, 
    EDX_SS           = 1 << 27, 
    EDX_HTT          = 1 << 28, 
    EDX_TM1          = 1 << 29, 
    EDX_IA64         = 1 << 30,
    EDX_PBE          = 1 << 31
};