#include "kernel/dev/devices.h"
#include "kernel/dev/framebuffer.h"
#include "kernel/dev/null.h"
#include "kernel/dev/zero.h"
#include "kernel/fs/filesystem.h"

void devices_initialise(uniheader* header)
{
    filesystem_info_t fs;
    
    fs.link(KERNEL_DEVICE_FRAMEBUFFER, new framebuffer_device(header->framebuffer.addr, header->framebuffer.width, header->framebuffer.height, header->framebuffer.pitch, header->framebuffer.bpp));
    fs.link(KERNEL_DEVICE_NULL, new null_device());
    fs.link(KERNEL_DEVICE_ZERO, new zero_device());
}
