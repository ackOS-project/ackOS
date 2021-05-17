#pragma once
#include <stdint.h>
#include <stddef.h>

enum class keyboard_scancode : uint32_t
{
    INVALID = 0x0,
    ESC = 0x01,

    NUM1 = 0x02,
    NUM2 = 0x03,
    NUM3 = 0x04,
    NUM4 = 0x05,
    NUM5 = 0x06,
    NUM6 = 0x07,
    NUM7 = 0x08,
    NUM8 = 0x09,
    NUM9 = 0x0A,
    NUM0 = 0x0B,

    SYM1 = 0x0C,
    SYM2 = 0x0D,

    BACKSPACE = 0x0E,
    TAB = 0x0F,

    Q = 0x10,
    W = 0x11,
    E = 0x12,
    R = 0x13,
    T = 0x14,
    Y = 0x15,
    U = 0x16,
    I = 0x17,
    O = 0x18,
    P = 0x19,

    SYM3 = 0x1A,
    SYM4 = 0x1B,
    ENTER = 0x1C,
    CTRL_LEFT = 0x1D,

    A = 0x1E,
    S = 0x1F,
    D = 0x20,
    F = 0x21,
    G = 0x22,
    H = 0x23,
    J = 0x24,
    K = 0x25,
    L = 0x26,

    SYM5 = 0x27,
    SYM6 = 0x28,
    SYM7 = 0x29,

    SHIFT_LEFT = 0x2A,
    SYM8 = 0x2B,

    Z = 0x2C,
    X = 0x2D,
    C = 0x2E,
    V = 0x2F,
    B = 0x30,
    N = 0x31,
    M = 0x32,

    SYM9 = 0x33,
    SYM10 = 0x34,
    SYM11 = 0x35,

    SHIFT_RIGHT = 0x36,
    SYM12 = 0x37,
    ALT_LEFT = 0x38,
    
    SPACE = 0x39,
    CAPSLOCK = 0x3A,

    F1 = 0x3B, 
    F2 = 0x3C, 
    F3 = 0x3D, 
    F4 = 0x3E, 
    F5 = 0x3F, 
    F6 = 0x40, 
    F7 = 0x41, 
    F8 = 0x42, 
    F9 = 0x43, 
    F10 = 0x44,

    NUMLOCK = 0x45,
    SCROLLLOCK = 0x46,

    KPAD7 = 0x47,
    KPAD8 = 0x48,
    KPAD9 = 0x49,
    SYM13 = 0x4A,
    KPAD4 = 0x4B,
    KPAD5 = 0x4C,
    KPAD6 = 0x4D,
    
    SYM14 = 0x4E,
    
    KPAD1 = 0x4F,
    KPAD2 = 0x50,
    KPAD3 = 0x51,
    KPAD0 = 0x52,

    SYM15 = 0x53,
    ALTSYSRQ = 0x54,

    SYM16 = 0x55,
    SYM17 = 0x56,

    F11 = 0x57,
    F12 = 0x58,

    COUNT
};

enum class key_state
{
    DOWN,
    RELEASED
};

keyboard_scancode kbps2_get_scancode();
char kbps2_scancode_to_ascii(keyboard_scancode scancode);
key_state kbps2_get_key_state(keyboard_scancode scancode);
