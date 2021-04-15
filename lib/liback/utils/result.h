#pragma once

namespace utils
{
    class result
    {
    private:
        int _code;

        #define ENUM_MACRO(X)                               \
            X(SUCCESS, "success")                            \
            X(ERR_UNIMPLEMENTED, "unimplemented function")    \
            X(ERR_OUT_OF_BOUNDS, "out of bounds")              \
            X(ERR_OUT_OF_MEMORY, "out of memory")               \
            X(ERR_INVALID_ADDRESS, "invalid address")            \
            X(ERR_INVALID_ARGUMENT, "invalid argument")           \
            X(ERR_INVALID_FD, "invalid file descriptor")           \
            X(ERR_NOT_READABLE, "not readable")                     \
            X(ERR_NOT_WRITABLE, "not writable")                      \
            X(ERR_UNKNOWN, "unknown error")

        #define GENERATE_ENUM(name, description) name,
        #define GENERATE_STR(name, description) #name,
        #define GENERATE_DESC(name, description) description,

        const char* _enum_strings[15] =
        {
            ENUM_MACRO(GENERATE_STR)
        };

        const char* _descriptions[15] =
        {
            ENUM_MACRO(GENERATE_DESC)
        };

    public:
        enum
        {
            ENUM_MACRO(GENERATE_ENUM)
        };

        #undef GENERATE_ENUM
        #undef GENERATE_DESC
        #undef ENUM_MACRO


        result();
        result(int code);

        result& operator=(int code);
        bool operator==(int code);
        bool operator!=(int code);

        const char* to_string();
        const char* description();

        int get_error_code();
    };
}
