#include "kernel/arch/x86_64/features/cpuid.h"
#include "kernel/arch/x86_64/features/sse.h"

namespace x86_64
{
    bool sse_check()
    {
        uint32_t edx = 0, unused = 0;

        __get_cpuid(1, &unused, &unused, &unused, &edx);

        return edx & cpuid::FEAT_EDX_SSE;
    }

    void sse_enable()
    {
        if(sse_check())
        {
            __asm__ __volatile__(
            "	movq %cr0, %rax\n"
            "	andb $0xF1, %al\n"
            "	movq %rax, %cr0\n"
            "	movq %cr4, %rax\n"
            "	orw $3 << 9, %ax\n"
            "	mov %rax, %cr4\n"
            );
        }
    }
}