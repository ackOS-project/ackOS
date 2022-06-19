#pragma once

#include <cstdint>

#include "kernel/io/fs_node.h"

#define KERNEL_DEVICE_FRAMEBUFFER "/dev/fb0"

struct framebuffer_write_data_info
{
    uint32_t* buff = nullptr;
    uint32_t buff_width = 0;
    uint32_t buff_height = 0;

    int x = 0;
    int y = 0;
};

struct framebuffer_get_fb_info
{
    int width = 0;
    int height = 0;
    int pitch = 0;
    int bpp = 0;
};

enum
{
    FRAMEBUFFER_CALL_WRITE_DATA,
    FRAMEBUFFER_CALL_GET_DISPLAY_INFO
};

class framebuffer_device : public fs_node
{
private:
    void* _addr_virtual;
    int _width;
    int _height;
    int _pitch;
    int _bpp;

public:
    framebuffer_device(void* addr_virtual, int width, int height, int pitch, int bpp);
    ~framebuffer_device();

    utils::result io_call(int request, void* arg) override;
};
