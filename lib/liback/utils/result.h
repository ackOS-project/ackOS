#pragma once

namespace utils
{
    class result
    {
    private:
        int _code = ERR_UNKNOWN;

    public:
        #include "liback/utils/__result_macro.h"
        #define RESULT_GENERATE_ENUM(__name, __description) __name,

        enum
        {
            RESULT_MACRO(RESULT_GENERATE_ENUM)
        };

        #undef GENERATE_ENUM
        #undef GENERATE_DESC
        #undef RESULT_MACRO

        result();
        result(int code);

        result& operator=(int code);
        bool operator==(int code);
        bool operator!=(int code);

        explicit operator int();
        operator bool();

        const char* to_string();
        const char* get_description();

        int get_error_code();
    };

    template <typename T>
    class result_tuple
    {
    private:
        result _result;
        T _data;
    public:
        result_tuple(result res)
        :
        _result(res)
        {
        }

        result_tuple(T data)
        :
        _result(result::SUCCESS),
        _data(data)
        {
        }

        operator bool()
        {
            return _result == result::SUCCESS;
        }

        result get_result()
        {
            return _result;
        }

        T get_value()
        {
            return _data;
        }
    };
}
