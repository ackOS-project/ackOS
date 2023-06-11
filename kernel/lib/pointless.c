#include "kernel/lib/pointless.h"

intmax_t fraction_to_percentage(intmax_t num, intmax_t dem)
{
    if(dem == 0) return 0;

    intmax_t x = num / dem;
    intmax_t rem = num % dem;

    // basically long division
    for(int i = 0; i < 2; i++)
    {
        rem *= 10;

        int digit = rem / dem;

        x *= 10;
        x += digit;

        rem %= dem;
    }

    // round up
    rem *= 10;

    int last_digit = rem / dem;

    if(last_digit >= 5)
    {
        x++;
    }

    return x;
}
