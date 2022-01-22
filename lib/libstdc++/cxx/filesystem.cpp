#include <filesystem>
#include <liback/utils/lexer.h>

namespace std::filesystem
{
    void path::split(const string_type& s)
    {
        _components.clear();

        utils::lexer lexer(s);
        std::string component;

        while(lexer)
        {
            if(lexer.current() == preferred_separator)
            {
                if(!component.empty())
                {
                    _components.push_back(component);
                    component.clear();
                }
                else if(lexer.peek(-1) != preferred_separator)
                {
                    _components.push_back(std::string(1, preferred_separator));
                }
            }
            else
            {
                component.append(1, lexer.current());
            }

            lexer.advance();
        }

        if(!component.empty()) _components.push_back(component);
    }
}
