#pragma once

#include <string>

namespace utils
{
    class lexer
    {
    private:
        using char_type = char;
        using string_type = std::string;

        string_type _source;
        int _index = 0;

    public:
        class iterator
        {
        private:
            const string_type& _s;
            int _i = 0;

            iterator(const string_type& s, int i)
            :
            _s(s),
            _i(i)
            {
            }

            iterator()
            :
            _s(nullptr),
            _i(-1)
            {
            }

            char_type get(int i) const
            {
                if(i >= _s.size() || i < 0) return 0;

                return _s[i];
            }

        public:
            operator const char_type() const
            {
                return get(_i);
            }

            int line_number()
            {
                int line = 1;

                for(int i = 0; i < _i; i++)
                {
                    char_type c = get(i);

                    if(c == '\n')
                    {
                        line++;
                    }
                    else if(c == 0)
                    {
                        break;
                    }
                }

                return line;
            }

            int column_number()
            {
                int column = 0;

                for(int i = 0; i < _i; i++)
                {
                    char_type c = get(i);

                    if(c == '\n')
                    {
                        column = 0;
                    }
                    else if(c == 0)
                    {
                        break;
                    }
                    else
                    {
                        column++;
                    }
                }

                return column;
            }

            iterator operator++(int)
            {
                iterator temp = *this;

                ++(*this);

                return temp;
            }

            iterator& operator++()
            {
                _i++;

                return *this;
            }

            iterator operator--(int)
            {
                iterator temp = *this;

                --(*this);

                return temp;
            }

            iterator& operator--()
            {
                _i--;

                return *this;
            }

            bool operator==(const iterator& rhs) const
            {
                return rhs._i == _i && rhs._s == _s;
            }

            bool operator==(const string_type& rhs) const
            {
                for(int i = 0; i < rhs.size(); i++)
                {
                    if(get(_i + i) != rhs[i])
                    {
                        return false;
                    }
                }

                return true;
            }

            bool is(const string_type& chars) const
            {
                if(get(_i) == 0) { return false; }

                for(char_type c : chars)
                {
                    if(get(_i) == c)
                    {
                        return true;
                    }
                }

                return false;
            }

            bool is_not(const string_type& chars) const
            {
                return is(chars) == false;
            }

            bool operator!=(const iterator& rhs) const
            {
                return rhs._i != _i || rhs._s != _s;
            }

            const char_type operator*() const
            {
                return get(_i);
            }

            friend lexer;
        };

        lexer(const string_type& s)
        :
        _source(s)
        {
        }

        lexer()
        {
        }

        void set_source(const string_type& s)
        {
            _source = s;
        }

        iterator current()
        {
            return iterator(_source, _index);
        }

        const iterator current() const
        {
            return iterator(_source, _index);
        }

        iterator peek(int how_many = 1)
        {
            return iterator(_source, _index + how_many);
        }

        const iterator peek(int how_many = 1) const
        {
            return iterator(_source, _index + how_many);
        }

        iterator advance(int how_many = 1)
        {
            return iterator(_source, _index += how_many);
        }

        lexer& skip(char_type c)
        {
            while(current() == c)
            {
                advance();
            }

            return *this;
        }

        lexer& skip(const string_type& s)
        {
            while(current() == s)
            {
                advance(s.size());
            }

            return *this;
        }

        iterator begin()
        {
            return iterator(_source, 0);
        }

        iterator end()
        {
            return iterator(_source, _source.size());
        }

        operator bool() const
        {
            return (char)current() != 0;
        }
    };
}
