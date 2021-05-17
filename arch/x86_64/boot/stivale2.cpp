#include "kernel/boot_protocols/stivale2.h"

extern "C" int x86_64_init(void* header, uint32_t magic);

void hello()
{
    return;
}

extern "C" void _stivale2_start(void* header)
{
    x86_64_init(header, 0xC0DE);

    __asm__ __volatile__("cli;"
                         "hlt;");
}

extern uint64_t _kernel_stack_top;

static stivale2_header_tag_framebuffer framebuffer_hdr_tag =
{
    .tag =
    {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};

__attribute__((section(".stivale2hdr"), used))
static stivale2_header _stivale2_header =
{
    .entry_point = (uint64_t)&_stivale2_start,
    .stack = (uintptr_t)&_kernel_stack_top,
    .flags = 0,
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

