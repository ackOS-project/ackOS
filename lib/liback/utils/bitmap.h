#pragma once

#include <cstdint>
#include <cstddef>

namespace utils
{
    class bitmap
    {
    private:
        size_t _size;
        uint8_t* _buff;

    public:
        bitmap(uint8_t* buff, size_t size) :
        _buff(buff),
        _size(size)
        {
        }

        size_t get_size() { return _size; }

        bool get(int index);
        void set(int index, bool value);
    };
}
