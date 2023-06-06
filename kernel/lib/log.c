#include <liback/util.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "kernel/lib/log.h"

enum
{
    PRINTF_STATE_NORMAL,
    PRINTF_STATE_FLAG,
    PRINTF_STATE_WIDTH,
    PRINTF_STATE_LENGTH,
    PRINTF_STATE_SPECIFIER
};

enum
{
    PRINTF_FLAG_DEFAULT,
    PRINTF_FLAG_RIGHT_JUSTIFY,
    PRINTF_FLAG_PAD_ZEROES = 1 << 1,
    PRINTF_FLAG_SHOW_SIGN = 1 << 2,
    PRINTF_FLAG_ALTERNATE_FORM = 1 << 3,
    PRINTF_FLAG_DIGIT_SEPARATE = 1 << 4
};

enum
{
    PRINTF_LEN_DEFAULT,
    PRINTF_LEN_SHORT,
    PRINTF_LEN_SHORT_SHORT,
    PRINTF_LEN_LONG,
    PRINTF_LEN_LONG_LONG,
    PRINTF_LEN_SIZE_TYPE
};


// an unoffical feauture which converts plurals in parentheses
// to the corresponding pluralisation/single for a number
// example:
// ("%i cat(s)", 1) => "1 cat"
// ("%i cat(s)", 3) => "3 cats"
enum
{
    PRINTF_PLURAL_DEFAULT,
    PRINTF_PLURAL_YES,
    PRINTF_PLURAL_NO
};

#define FRAC_MAX_PRECISION 2

// Using a macro here, so we can duplicate it for different interger types
#define INT_TO_STRING_FUNC(T_, NAME_) \
static size_t NAME_(T_ value, char* dest, int base, int padding, char pad_char, const char* prefix, char digit_separator, bool show_pos_sign, bool use_uppercase, bool quantify, bool dry_run) \
{ \
    size_t len = 0; \
    static const char lookup_table[] = "0123456789abcdefghijklmnopqrstuvwxyz"; \
    if(base < 1 || base > (sizeof(lookup_table) / sizeof(char))) \
    { \
        if(!dry_run) *dest = '\0'; \
        return 0; \
    } \
    size_t quantifier_size = 1; \
    size_t quantifier_frac = 0; \
    const char* quantifier_name = NULL; \
    if(quantify) \
    { \
        size_t nbytes = (size_t)value; \
        if(nbytes >= (1 TiB)) \
        { \
            quantifier_size = (1 TiB); \
            quantifier_name = "TiB"; \
        } \
        else if(nbytes >= (1 GiB)) \
        { \
            quantifier_size = (1 GiB); \
            quantifier_name = "GiB"; \
        } \
        else if(nbytes >= (1 MiB)) \
        { \
            quantifier_size = (1 MiB); \
            quantifier_name = "MiB"; \
        } \
        else if(nbytes >= (1 KiB)) \
        { \
            quantifier_size = (1 KiB); \
            quantifier_name = "KiB"; \
        } \
        else \
        { \
            quantifier_name = "B"; \
        } \
        size_t v = nbytes / quantifier_size; \
        size_t r = nbytes % quantifier_size; \
        if(r != 0) \
        { \
            size_t i = 0; \
            /* add the factional part to the value */ \
            for (; i <= FRAC_MAX_PRECISION && r != 0; i++) \
            { \
                r *= base; \
                int d = r / quantifier_size; \
                v *= base; \
                v += d; \
                r %= quantifier_size; \
            } \
            /* round up */ \
            int last_digit = v % base; \
            if(last_digit >= (base / 2)) \
            { \
                v += base - last_digit; \
            } \
            else \
            { \
                /* get rid of the digit */ \
                v -= last_digit; \
            } \
            /* separate the whole number and fractional part */ \
            for (size_t j = 0; j < i; j++) \
            { \
                int digit = v % base; \
                quantifier_frac *= base; \
                quantifier_frac += digit; \
                v /= base; \
            } \
        } \
        value = (T_)v; \
    } \
    size_t prefix_len = 0; \
    size_t sign_len = 0; \
    if(value < 0) \
    { \
        if(!dry_run) *(dest++) = '-'; \
        sign_len++; \
    } \
    else if(show_pos_sign) \
    { \
        if(!dry_run) *(dest++) = '+'; \
        sign_len++; \
    } \
    if(prefix) \
    { \
        prefix_len = strlen(prefix); \
        if(!dry_run) \
        { \
            memcpy(dest, prefix, prefix_len); \
            dest += prefix_len; \
        } \
    } \
    char* num_start = dest; \
    T_ n = ABS(value); \
    /* take digits and write them to the buffer */ \
    /* if the value is 0 then write a 0 */ \
    if(value == 0) \
    { \
        if(!dry_run) *(dest++) = '0'; \
        len++; \
    } \
    for(size_t i = 1; n != 0; i++) \
    { \
        if(!dry_run) \
        { \
            int digit = n % base; \
            char c = use_uppercase ? toupper(lookup_table[digit]) : lookup_table[digit]; \
            *(dest++) = c; \
        } \
        len++; \
        n /= base; \
        if(i % 3 == 0 && digit_separator && n != 0) \
        { \
            if(!dry_run) *(dest++) = digit_separator; \
            len++;\
        } \
    } \
    if(padding && pad_char) \
    { \
        size_t real_len = len + prefix_len + sign_len; \
        size_t padding_len = padding < real_len ? 0 : padding - real_len; \
        if(!dry_run) \
        { \
            memset(dest, pad_char, padding_len); \
            dest += padding_len; \
        } \
        len += padding_len; \
    } \
    if(!dry_run) \
    { \
        /* reverse the number buffer, because it is the wrong way round */ \
        for(size_t i = 0; i < len / 2; i++) \
        { \
            char* low = &num_start[i]; \
            char* high = &num_start[len - i - 1]; \
            char lowc = *low, highc = *high; \
            *low = highc; \
            *high = lowc; \
        } \
    } \
    len += prefix_len; \
    len += sign_len; \
    if(quantifier_frac != 0) \
    { \
        if(!dry_run) *(dest++) = '.'; \
        len++; \
        while(quantifier_frac != 0) \
        { \
            if(!dry_run) \
            { \
                int digit = quantifier_frac % base; \
                char c = use_uppercase ? toupper(lookup_table[digit]) : lookup_table[digit]; \
                *(dest++) = c; \
            } \
            len++; \
            quantifier_frac /= base; \
        } \
    } \
    if(quantifier_name) \
    { \
        size_t quantifier_len = strlen(quantifier_name); \
        if(!dry_run) \
        { \
            memcpy(dest, quantifier_name, quantifier_len); \
            dest += quantifier_len; \
        } \
        len += quantifier_len; \
    } \
    if(!dry_run) \
    { \
        *dest = '\0'; \
    } \
    return len; \
}

INT_TO_STRING_FUNC(signed char, char_to_string)
INT_TO_STRING_FUNC(unsigned char, uchar_to_string)
INT_TO_STRING_FUNC(signed int, int_to_string)
INT_TO_STRING_FUNC(unsigned int, uint_to_string)
INT_TO_STRING_FUNC(unsigned short, usint_to_string)
INT_TO_STRING_FUNC(signed short, sint_to_string)
INT_TO_STRING_FUNC(signed long, lint_to_string)
INT_TO_STRING_FUNC(unsigned long, ulint_to_string)
INT_TO_STRING_FUNC(long long, llint_to_string)
INT_TO_STRING_FUNC(unsigned long long, ullint_to_string)

size_t get_print_width(const char* s)
{
    size_t len = 0;

    while(*s)
    {
        if(s[0] == '\e' && s[1] == '[')
        {
            while(s[0] && s[0] != 'm') s++;

            if(s[0]) s++;

            continue;
        }

        s++;
        len++;
    }

    return len;
}

static int dvsnprintf(char* buff, size_t n, size_t* written_len, const char* prefix, const char* fmt, bool dry_run, va_list args)
{
    char* buff_start = buff;
    int total_len = 0;
    size_t indent = 0;
    int state = PRINTF_STATE_NORMAL;
    int flag = PRINTF_FLAG_DEFAULT;
    int width = 0;
    int length = PRINTF_LEN_DEFAULT;
    int pluralisation = PRINTF_PLURAL_DEFAULT;

    #define PRINTF_GETSPACE() (n - (size_t)((uintptr_t)buff - (uintptr_t)buff_start))
    #define PRINTF_TRUNCATE(__len) (PRINTF_GETSPACE() < ((size_t)(__len)) ? PRINTF_GETSPACE() : ((size_t)(__len)))
    #define PRINTF_APPENDCH(__ch) \
        if(PRINTF_GETSPACE() > 0 && !dry_run) \
        { \
            *(buff++) = (__ch); \
        } \
        total_len++;
    #define PRINTF_JUSTIFYR(__len) \
        if(flag & PRINTF_FLAG_RIGHT_JUSTIFY) \
        { \
            size_t len = width >= (__len) ? width - (__len) : 0; \
            size_t pad_len = PRINTF_TRUNCATE(len); \
            if(!dry_run) \
            { \
                memset(buff, ' ', pad_len); \
                buff += pad_len; \
            } \
            total_len += len; \
        }
    #define PRINTF_JUSTIFYL(__len) \
        if(!(flag & PRINTF_FLAG_PAD_ZEROES) && !(flag & PRINTF_FLAG_RIGHT_JUSTIFY)) \
        { \
            size_t len = width >= (__len) ? width - (__len) : 0; \
            size_t pad_len = PRINTF_TRUNCATE(len); \
            if(!dry_run) \
            { \
                memset(buff, ' ', pad_len); \
                buff += pad_len; \
            } \
            total_len += len; \
        }

    if(prefix)
    {
        size_t prefix_len = strlen(prefix);

        if(!dry_run)
        {
            size_t trunc_len = PRINTF_TRUNCATE(prefix_len);

            strncpy(buff, prefix, trunc_len);
            buff += trunc_len;
        }

        total_len += prefix_len;
        indent = get_print_width(prefix);
    }

    while(*fmt)
    {
        switch(state)
        {
            case PRINTF_STATE_NORMAL:
                if(*fmt == '%')
                {
                    state = PRINTF_STATE_FLAG;
                    pluralisation = PRINTF_PLURAL_DEFAULT;
                    fmt++;

                    continue;
                }
                else if(pluralisation != PRINTF_PLURAL_DEFAULT && *fmt == '(' && *(fmt + 1) == 's' && *(fmt + 2) == ')')
                {
                    // ^ the above code is fine
                    fmt += 3;

                    if(pluralisation == PRINTF_PLURAL_YES)
                    {
                        PRINTF_APPENDCH('s');
                    }
                }
                else
                {
                    PRINTF_APPENDCH(*fmt);

                    if(indent > 0 && *fmt == '\n' && *(fmt + 1))
                    {
                        size_t indent_len = PRINTF_TRUNCATE(indent);

                        if(!dry_run)
                        {
                            memset(buff, ' ', indent_len);
                            buff += indent_len;
                        }

                        total_len += indent;
                    }

                    fmt++;
                }

                break;

            case PRINTF_STATE_FLAG:
            {
                bool is_flag = true;

                while(is_flag)
                {
                    switch(*fmt)
                    {
                        case '-':
                            flag |= PRINTF_FLAG_RIGHT_JUSTIFY;
                            fmt++;

                            break;
                        case '0':
                            flag |= PRINTF_FLAG_PAD_ZEROES;
                            fmt++;

                            break;
                        case '+':
                            flag |= PRINTF_FLAG_SHOW_SIGN;
                            fmt++;

                            break;
                        case '#':
                            flag |= PRINTF_FLAG_ALTERNATE_FORM;
                            fmt++;

                            break;
                        case '\'':
                            flag |= PRINTF_FLAG_DIGIT_SEPARATE;
                            fmt++;
                            
                            break;
                        default:
                            is_flag = false;

                            break;
                    }
                }

                break;
            }
            case PRINTF_STATE_WIDTH:
            {
                if(*fmt >= '1' && *fmt <= '9')
                {
                    char* endp = NULL;
                    width = strtol(fmt, &endp, 10);
                    fmt += (size_t)endp - (size_t)fmt;
                }
                else if(*fmt == '*')
                {
                    width = va_arg(args, int);
                    fmt++;
                }

                break;
            }
            case PRINTF_STATE_LENGTH:
                switch(*fmt)
                {
                    case 'h':
                        length = PRINTF_LEN_SHORT;

                        if(*(fmt + 1) == 'h')
                        {
                            length = PRINTF_LEN_SHORT_SHORT;

                            fmt++;
                        }

                        fmt++;

                        break;

                    case 'l':
                        length = PRINTF_LEN_LONG;

                        if(*(fmt + 1) == 'l')
                        {
                            length = PRINTF_LEN_LONG_LONG;

                            fmt++;
                        }

                        fmt++;

                        break;
                    case 'z':
                        length = PRINTF_LEN_SIZE_TYPE;

                        fmt++;

                        break;
                    default:
                        break;
                }

                break;
            
            case PRINTF_STATE_SPECIFIER:
            {
                switch(*fmt)
                {
                    case 'c':
                    {
                        char c = (char)va_arg(args, int);

                        PRINTF_JUSTIFYL(1);
                        PRINTF_APPENDCH(c);
                        PRINTF_JUSTIFYR(1);

                        break;
                    }
                    case 's':
                    {
                        char* str = va_arg(args, char*);

                        if(str)
                        {
                            size_t str_len = strlen(str);
                            size_t trunc_len = PRINTF_TRUNCATE(str_len);

                            PRINTF_JUSTIFYL(str_len);

                            if(!dry_run)
                            {
                                strncpy(buff, str, trunc_len);
                                
                                buff += trunc_len;
                            }

                            total_len += str_len;

                            PRINTF_JUSTIFYR(str_len);
                        }
                        else
                        {
                            const char* msg = "(null)";
                            size_t msg_len = strlen(str);
                            size_t trunc_len = PRINTF_TRUNCATE(msg_len);

                            PRINTF_JUSTIFYL(msg_len);

                            if(!dry_run)
                            {
                                strncpy(buff, msg, trunc_len);
                                buff += trunc_len;

                            }

                            total_len += msg_len;

                            PRINTF_JUSTIFYR(msg_len);
                        }

                        break;
                    }
                    case '%':
                    {
                        PRINTF_JUSTIFYR(1);
                        PRINTF_APPENDCH('%');

                        break;
                    }
                    #define PRINTF_INT_SPEC(__type, __vtype, __func, __signedness, __base, __prefix, __use_uppercase) \
                        __type value = (__type)va_arg(args, __vtype); \
                        size_t est_size = __func(value, NULL, (__base), width, flag & PRINTF_FLAG_PAD_ZEROES ? '0' : '\0', (__prefix), flag & PRINTF_FLAG_DIGIT_SEPARATE ? ' ' : '\0', flag & PRINTF_FLAG_SHOW_SIGN, (__use_uppercase), length == PRINTF_LEN_SIZE_TYPE && (flag & PRINTF_FLAG_DIGIT_SEPARATE), true); \
                        PRINTF_JUSTIFYL(est_size); \
                        size_t trunc_len = PRINTF_TRUNCATE(est_size); \
                        char tmp[est_size + 1]; \
                        __func(value, tmp, (__base), width, flag & PRINTF_FLAG_PAD_ZEROES ? '0' : '\0', (__prefix), flag & PRINTF_FLAG_DIGIT_SEPARATE ? ' ' : '\0', flag & PRINTF_FLAG_SHOW_SIGN, (__use_uppercase), length == PRINTF_LEN_SIZE_TYPE && (flag & PRINTF_FLAG_DIGIT_SEPARATE), false); \
                        if(!dry_run) \
                        { \
                            strncpy(buff, tmp, trunc_len); \
                            buff += trunc_len; \
                        } \
                        total_len += est_size; \
                        PRINTF_JUSTIFYR(est_size); \
                        if(((__signedness) && value < -1) || value == 0 || value > 1) \
                        { \
                            pluralisation = PRINTF_PLURAL_YES; \
                        } \
                        else \
                        { \
                            pluralisation = PRINTF_PLURAL_NO; \
                        }
                    #define PRINTF_LEN_SPEC(__base, __use_uppercase, __prefix, __signedness) \
                        switch(length) \
                        { \
                            case PRINTF_STATE_NORMAL: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(signed int, signed int, int_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(unsigned int, unsigned int, uint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_SHORT: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(signed short, signed int, sint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(unsigned short, unsigned int, usint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_SHORT_SHORT: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(signed char, signed int, char_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(unsigned char, unsigned int, uchar_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_LONG_LONG: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(signed long long, signed long long, llint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(unsigned long long, unsigned long long, ullint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_LONG: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(signed long, signed long, lint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(unsigned long, unsigned long, ulint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            case PRINTF_LEN_SIZE_TYPE: \
                            { \
                                if(__signedness == true) \
                                { \
                                    PRINTF_INT_SPEC(ssize_t, ssize_t, lint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                else \
                                { \
                                    PRINTF_INT_SPEC(size_t, size_t, ulint_to_string, __signedness, __base, __prefix, __use_uppercase) \
                                } \
                                break; \
                            } \
                            default: \
                                break; \
                        }

                    case 'i':
                    case 'd':
                    {
                        PRINTF_LEN_SPEC(10, false, NULL, true);

                        break;
                    }
                    case 'u':
                    {
                        PRINTF_LEN_SPEC(10, false, NULL, false);

                        break;
                    }
                    case 'o':
                    {
                        const char* prefix = flag & PRINTF_FLAG_ALTERNATE_FORM ? "0o" : NULL;

                        PRINTF_LEN_SPEC(8, false, prefix, false);

                        break;
                    }
                    case 'x':
                    {
                        const char* prefix = flag & PRINTF_FLAG_ALTERNATE_FORM ? "0x" : NULL;

                        PRINTF_LEN_SPEC(16, false, prefix, false);

                        break;
                    }
                    case 'X':
                    {
                        const char* prefix = flag & PRINTF_FLAG_ALTERNATE_FORM ? "0x" : NULL;

                        PRINTF_LEN_SPEC(16, true, prefix, false);

                        break;
                    }
                    case 'p':
                    {
                        PRINTF_INT_SPEC(unsigned long, unsigned long, ulint_to_string, false, 16, "0x", false);

                        break;
                    }
                    default:
                        break;
                }

                fmt++;
                break;
            }

            default:
                break;
        }

        if(state != PRINTF_STATE_NORMAL && state < PRINTF_STATE_SPECIFIER)
        {
            state++;
        }
        else
        {
            state = PRINTF_STATE_NORMAL;
            flag = PRINTF_FLAG_DEFAULT;
            width = 0;
            length = PRINTF_LEN_DEFAULT;
        }
    }

    if(written_len && !dry_run)
    {
        *written_len = buff - buff_start;
    }

    if(!dry_run) *buff = '\0';

    #undef PRINTF_APPENDCH
    #undef PRINTF_JUSTIFYR
    #undef PRINTF_TRUNCATE
    #undef PRINTF_INT_SPEC
    #undef PRINTF_LEN_SPEC

    return total_len;
}

int vsnprintf(char* buff, size_t n, const char* fmt, va_list args)
{
    size_t written_len = 0;

    dvsnprintf(buff, n, &written_len, (const char*)NULL, fmt, false, args);

    return written_len;
}

int vsprintf(char* buff, const char* fmt, va_list args)
{
    return vsnprintf(buff, SIZE_MAX, fmt, args);
}

int snprintf(char* buff, size_t n, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsnprintf(buff, n, fmt, args);

    va_end(args);

    return length;
}

int sprintf(char* buff, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int length = vsprintf(buff, fmt, args);

    va_end(args);

    return length;
}

enum kprintf_action
{
    KPRINTF_ACTION_NONE,
    KPRINTF_ACTION_INFO,
    KPRINTF_ACTION_WARN,
    KPRINTF_ACTION_PANIC,
    KPRINTF_ACTION_DEBUG
};

static const char* action_to_string(enum kprintf_action action)
{
    switch(action)
    {
    case KPRINTF_ACTION_INFO:
        return TERM_CYAN "INFO" TERM_RESET_COLOUR "  ";
        break;
    case KPRINTF_ACTION_WARN:
        return TERM_YELLOW "WARN" TERM_RESET_COLOUR "  ";
        break;
    case KPRINTF_ACTION_DEBUG:
        return TERM_MAGENTA "DEBUG" TERM_RESET_COLOUR " ";
        break;
    case KPRINTF_ACTION_PANIC:
        return TERM_RED_BG "PANIC" TERM_RESET_COLOUR " ";
        break;

    default:
        break;
    }

    return (const char*)NULL;
}

int kvprintf(const char* fmt, va_list args)
{
    va_list args_clone;
    va_copy(args_clone, args);

    const char* status_msg = (const char*)NULL;
    enum kprintf_action action = KPRINTF_ACTION_NONE;

    size_t fmt_len = strlen(fmt);

    if(fmt_len >= 2 && fmt[0] == '\e')
    {
        action = fmt[1];
        status_msg = action_to_string(action);

        if(status_msg)
        {
            fmt += 2;
        }
    }

#ifndef BUILD_CONFIG_DEBUG
    if(action == KPRINTF_ACTION_DEBUG)
    {
        return 0;
    }
#endif

    size_t written_len = 0;
    size_t est_len = dvsnprintf((char*)NULL, 0, (size_t*)NULL, status_msg, fmt, true, args_clone);

    char buff[est_len + 1];

    dvsnprintf(buff, est_len, &written_len, status_msg, fmt, false, args);
    kputs(buff);

    if(action == KPRINTF_ACTION_PANIC)
    {
        kpanic();
    }

    va_end(args_clone);

    return written_len;
}

int kprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int written_len = kvprintf(fmt, args);

    va_end(args);

    return written_len;
}
