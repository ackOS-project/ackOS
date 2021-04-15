#include <liback/utils/result.h>

namespace utils
{
    result::result() { }

    result::result(int code)
    {
        this->_code = code;
    }

    result& result::operator=(int code)
    {
        this->_code = code;

        return *this;
    }

    bool result::operator==(int code)
    {
        return this->_code == code;
    }

    bool result::operator!=(int code)
    {
        return this->_code != code;
    }

    const char* result::description()
    {
        return this->_descriptions[this->_code];
    }

    const char* result::to_string()
    {
        return this->_enum_strings[this->_code];
    }

    int result::get_error_code()
    {
        return this->_code;
    }
}
