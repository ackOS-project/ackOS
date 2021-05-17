#include "kernel/drivers/vbe.h"
#include "kernel/mm/heap.h"

void vbe_get_modes()
{
    vbe_header* header = (vbe_header*)heap_allocate(512);

    
}
