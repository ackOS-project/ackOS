#include "kernel/io.h"
#include "kernel/drivers/keyboard.h"

const char* keyboard_map[(uint32_t)keyboard_key::COUNT] =
{
    "", "", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "", "",
    "\b", "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "", ""
};

keyboard_key keyboard_get_keycode()
{
    if (inb(0x64) & 0x1)
    {
        return (keyboard_key)inb(0x60);
    }

    return keyboard_key::INVALID;
}
