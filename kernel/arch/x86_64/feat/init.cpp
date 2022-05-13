#include "kernel/arch/x86_64/feat/idt.h"
#include "kernel/arch/x86_64/feat/gdt.h"
#include "kernel/arch/x86_64/feat/pic_8259.h"
#include "kernel/arch/x86_64/feat/asm.h"
#include "kernel/arch/x86_64/feat/com.h"
#include "kernel/arch/x86_64/feat/sse.h"
#include "kernel/arch/x86_64/feat/paging.h"
#include "kernel/arch/x86_64/feat/acpi/apic.h"
#include "kernel/boot/uniheader.h"

#include <liback/utils/assert.h>

static uniheader uheader;
void kmain(uniheader&);

using namespace x86_64;

void __stdlib_init();

extern "C" int x86_64_init(void* header, uint32_t magic)
{
    __stdlib_init();

    com_initialize(COM1, 9600);
    com_initialize(COM2, 9600);
    com_initialize(COM3, 9600);
    com_initialize(COM4, 9600);

    gdt_initialise();
    idt_initialise();

    pic8259_remap();
    pic8259_mask();

    interrupts_enable();

    sse_enable();

    apic::initialise();

    uheader.parse(header, magic);

    kmain(uheader);

    utils::assert_if_reached("kmain() returned");

    return 0;
}
