#include "kernel/arch/x86_64/features/idt.h"
#include "kernel/arch/x86_64/features/gdt.h"
#include "kernel/arch/x86_64/features/pic_8259.h"
#include "kernel/arch/x86_64/features/instructions.h"
#include "kernel/arch/x86_64/features/com.h"
#include "kernel/arch/x86_64/features/sse.h"
#include "kernel/arch/x86_64/features/paging.h"

#include <liback/utils/assert.h>
#include "kernel/boot_protocols/uniheader.h"

static uniheader uheader;
void kmain(uniheader&);

using namespace x86_64;

extern "C" int x86_64_init(void* header, uint32_t magic)
{
    com_initialize(COM1, 9600);
    com_initialize(COM2, 9600);
    com_initialize(COM3, 9600);
    com_initialize(COM4, 9600);

    gdt_initialise();
    idt_initialise();

    pic8259_remap();
    pic8259_mask();

    interrupts_enable();

    //paging_initialise();

    sse_enable();

    uheader.parse(header, magic);

    kmain(uheader);

    utils::assert_if_reached("kmain() returned");

    return 0;
}
