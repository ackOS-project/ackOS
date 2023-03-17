#include <liback/util.h>

#include "kernel/arch/x86_64/cpuid.h"

void cpuid_do(uint32_t leaf, uint32_t* eax_p, uint32_t* ebx_p, uint32_t* ecx_p, uint32_t* edx_p)
{
    uint32_t eax, ebx, ecx, edx;

    __asm__ volatile("xchgq  %%rbx, %q1\n"
                     "cpuid\n"
                     "xchgq  %%rbx, %q1"
                    : "=a"(eax), "=r" (ebx), "=c"(ecx), "=d"(edx)
                    : "0"(leaf));

    if(eax_p) *eax_p = eax;
    if(ebx_p) *ebx_p = ebx;
    if(ecx_p) *ecx_p = ecx;
    if(edx_p) *edx_p = edx;
}

void cpuid_do_with_subleaf(uint32_t leaf, uint32_t subleaf, uint32_t* eax_p, uint32_t* ebx_p, uint32_t* ecx_p, uint32_t* edx_p)
{
    uint32_t eax, ebx, ecx, edx;

    __asm__ volatile("xchgq  %%rbx, %q1\n"
                     "cpuid\n"
                     "xchgq  %%rbx, %q1"
                    : "=a"(eax), "=r" (ebx), "=c"(ecx), "=d"(edx)
                    : "0"(leaf), "2"(subleaf));

    if(eax_p) *eax_p = eax;
    if(ebx_p) *ebx_p = ebx;
    if(ecx_p) *ecx_p = ecx;
    if(edx_p) *edx_p = edx;
}

uint32_t cpuid_eax(uint32_t leaf)
{
    uint32_t eax;

    cpuid_do(leaf, &eax, NULL, NULL, NULL);

    return eax;
}


uint32_t cpuid_ebx(uint32_t leaf)
{
    uint32_t ebx;

    cpuid_do(leaf, NULL, &ebx, NULL, NULL);

    return ebx;
}

uint32_t cpuid_ecx(uint32_t leaf)
{
    uint32_t ecx;

    cpuid_do(leaf, NULL, NULL, &ecx, NULL);

    return ecx;
}

uint32_t cpuid_edx(uint32_t leaf)
{
    uint32_t edx;

    cpuid_do(leaf, NULL, NULL, NULL, &edx);

    return edx;
}

uint32_t cpuid_eax_with_subleaf(uint32_t leaf, uint32_t subleaf)
{
    uint32_t eax;

    cpuid_do_with_subleaf(leaf, subleaf, &eax, NULL, NULL, NULL);

    return eax;
}


uint32_t cpuid_ebx_with_subleaf(uint32_t leaf, uint32_t subleaf)
{
    uint32_t ebx;

    cpuid_do_with_subleaf(leaf, subleaf, NULL, &ebx, NULL, NULL);

    return ebx;
}

uint32_t cpuid_ecx_with_subleaf(uint32_t leaf, uint32_t subleaf)
{
    uint32_t ecx;

    cpuid_do_with_subleaf(leaf, subleaf, NULL, NULL, &ecx, NULL);

    return ecx;
}

uint32_t cpuid_edx_with_subleaf(uint32_t leaf, uint32_t subleaf)
{
    uint32_t edx;

    cpuid_do_with_subleaf(leaf, subleaf, NULL, NULL, NULL, &edx);

    return edx;
}

char* cpuid_get_vendor_id(char res[static 13])
{
    uint32_t ebx, edx, ecx;

    cpuid_do(0, NULL, &ebx, &ecx, &edx);

    res[0] = ebx & 0xff;
    res[1] = ebx >> 8 & 0xff;
    res[2] = ebx >> 16 & 0xff;
    res[3] = ebx >> 24 & 0xff;

    res[4] = edx & 0xff;
    res[5] = edx >> 8 & 0xff;
    res[6] = edx >> 16 & 0xff;
    res[7] = edx >> 24 & 0xff;

    res[8] = ecx & 0xff;
    res[9] = ecx >> 8 & 0xff;
    res[10] = ecx >> 16 & 0xff;
    res[11] = ecx >> 24 & 0xff;

    res[12] = '\0';

    return res;
}

char* cpuid_get_brand_string(char res[static 49])
{
    uint32_t eax1, ebx1, ecx1, edx1;
    uint32_t eax2, ebx2, ecx2, edx2;
    uint32_t eax3, ebx3, ecx3, edx3;

    cpuid_do(0x80000002, &eax1, &ebx1, &ecx1, &edx1);
    cpuid_do(0x80000003, &eax2, &ebx2, &ecx2, &edx2);
    cpuid_do(0x80000004, &eax3, &ebx3, &ecx3, &edx3);

    res[0] = eax1 & 0xff;
    res[1] = eax1 >> 8 & 0xff;
    res[2] = eax1 >> 16 & 0xff;
    res[3] = eax1 >> 24 & 0xff;

    res[4] = ebx1 & 0xff;
    res[5] = ebx1 >> 8 & 0xff;
    res[6] = ebx1 >> 16 & 0xff;
    res[7] = ebx1 >> 24 & 0xff;

    res[8] = ecx1 & 0xff;
    res[9] = ecx1 >> 8 & 0xff;
    res[10] = ecx1 >> 16 & 0xff;
    res[11] = ecx1 >> 24 & 0xff;

    res[12] = edx1 & 0xff;
    res[13] = edx1 >> 8 & 0xff;
    res[14] = edx1 >> 16 & 0xff;
    res[15] = edx1 >> 24 & 0xff;

    res[16] = eax2 & 0xff;
    res[17] = eax2 >> 8 & 0xff;
    res[18] = eax2 >> 16 & 0xff;
    res[19] = eax2 >> 24 & 0xff;

    res[20] = ebx2 & 0xff;
    res[21] = ebx2 >> 8 & 0xff;
    res[22] = ebx2 >> 16 & 0xff;
    res[23] = ebx2 >> 24 & 0xff;

    res[24] = ecx2 & 0xff;
    res[25] = ecx2 >> 8 & 0xff;
    res[26] = ecx2 >> 16 & 0xff;
    res[27] = ecx2 >> 24 & 0xff;

    res[28] = edx2 & 0xff;
    res[29] = edx2 >> 8 & 0xff;
    res[30] = edx2 >> 16 & 0xff;
    res[31] = edx2 >> 24 & 0xff;

    res[32] = eax3 & 0xff;
    res[33] = eax3 >> 8 & 0xff;
    res[34] = eax3 >> 16 & 0xff;
    res[35] = eax3 >> 24 & 0xff;

    res[36] = ebx3 & 0xff;
    res[37] = ebx3 >> 8 & 0xff;
    res[38] = ebx3 >> 16 & 0xff;
    res[39] = ebx3 >> 24 & 0xff;

    res[40] = ecx3 & 0xff;
    res[41] = ecx3 >> 8 & 0xff;
    res[42] = ecx3 >> 16 & 0xff;
    res[43] = ecx3 >> 24 & 0xff;

    res[44] = edx3 & 0xff;
    res[45] = edx3 >> 8 & 0xff;
    res[46] = edx3 >> 16 & 0xff;
    res[47] = edx3 >> 24 & 0xff;

    res[48] = '\0';

    return res;
}
