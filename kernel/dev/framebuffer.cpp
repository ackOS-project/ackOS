#include <algorithm>

#include <cstdio>
#include "kernel/sys/logger.h"

#include "kernel/dev/framebuffer.h"

framebuffer_device::framebuffer_device(void* addr_physical, int width, int height, int pitch, int bpp)
:
fs_node(NODE_TYPE_DEVICE)
{
    _addr_virtual = addr_physical;
    _width = width;
    _height = height;
    _pitch = pitch;
    _bpp = bpp;
}

framebuffer_device::~framebuffer_device()
{
}

utils::result framebuffer_device::io_call(int request, void* arg)
{
    if(arg == nullptr)
    {
        return utils::result::ERR_INVALID_ADDRESS;
    }

    if(request == FRAMEBUFFER_CALL_WRITE_DATA)
    {
        const framebuffer_write_data_info* info = (framebuffer_write_data_info*)arg;

        uint32_t* framebuffer = (uint32_t*)_addr_virtual;

        for(int y = 0; y < info->buff_height; y++)
        {
            for(int x = 0; x < info->buff_width; x++)
            {
                uint32_t pixel_colour = info->buff[y * info->buff_width + x];
                uint32_t index = (info->y + y) * (_pitch / 4) + (info->x + x);

                if(index >= _width * _height)
                {
                    continue;
                }

                framebuffer[index] = pixel_colour;
            }
        }

        return utils::result::SUCCESS;
    }
    if(request == FRAMEBUFFER_CALL_GET_DISPLAY_INFO)
    {
        framebuffer_get_fb_info* info = (framebuffer_get_fb_info*)arg;

        info->width = _width;
        info->height = _height;
        info->pitch = _pitch;
        info->bpp = _bpp;

        return utils::result::SUCCESS;
    }
    else
    {
        return utils::result::ERR_INVALID_ARGUMENT;
    }
}
