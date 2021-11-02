#include "kernel/dev/framebuffer.h"

framebuffer_device::framebuffer_device(void* addr_virtual, int width, int height, int pitch, int bpp)
:
fs_node(NODE_TYPE_DEVICE)
{
    _addr_virtual = addr_virtual;
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

        for(int y = 0; y < info->buff_height; y++)
        {
            for(int x = 0; x < info->buff_width; x++)
            {
                int pixel_index = (info->y + y) * (_pitch / 4) + (info->x + x);
                uint32_t pixel_colour = info->buff[info->buff_width * y + x];

                ((uint32_t*)_addr_virtual)[pixel_index] = pixel_colour;
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
