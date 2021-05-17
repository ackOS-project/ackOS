#include "kernel/boot_protocols/uniheader.h"
#include "kernel/boot_protocols/multiboot2.h"
#include "kernel/boot_protocols/stivale2.h"
#include "kernel/panic.h"

void uniheader_parse(uniheader& uheader, void* header, uint32_t magic)
{
    if(stivale2_check_magic(magic))
    {
        stivale2_parse_tags(&uheader, header);
    }
    else if(multiboot2_check_magic(magic))
    {
        multiboot2_parse_tags(&uheader, header);
    }
    else
    {
        kpanic("no suitable boot protocol was found");
    }
}
