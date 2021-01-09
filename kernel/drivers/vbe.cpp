#include "kernel/drivers/vbe.h"
#include "kernel/mm/memory.h"

void vbe_get_modes()
{
    vbe_header* header = (vbe_header*)memory_alloc(512);

    
}
