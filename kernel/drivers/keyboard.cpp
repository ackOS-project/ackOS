#include "kernel/io.h"
#include "kernel/drivers/keyboard.h"

char keyboard_map[(uint32_t)keyboard_scancode::COUNT] =
{
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0, 0,
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 0, 0
};

keyboard_scancode keyboard_get_keycode()
{
    if(inb(0x64) & 0x1)
    {
        return (keyboard_scancode)inb(0x60);
    }

    return keyboard_scancode::INVALID;
}
