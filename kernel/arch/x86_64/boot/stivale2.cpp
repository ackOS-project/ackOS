#include "kernel/arch/x86_64/feat/asm.h"
#include "kernel/boot/stivale2.h"

extern "C" int x86_64_init(void* header, uint32_t magic);

static void _stivale2_start(void* header)
{
    x86_64_init(header, 0xC0DE);

    x86_64::interrupts_disable();
    x86_64::halt();
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

static stivale2_tag five_level_paging_tag =
{
    .identifier = STIVALE2_HEADER_TAG_5LV_PAGING_ID,
    .next = (uint64_t)&framebuffer_hdr_tag
};

__attribute__((section(".stivale2hdr"), used))
static stivale2_header _stivale2_header =
{
    .entry_point = (uint64_t)&_stivale2_start,
    .stack = (uintptr_t)&_kernel_stack_top,
    .flags = 0,
    .tags = (uintptr_t)&five_level_paging_tag
};

