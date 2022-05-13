#pragma once

#include <initializer_list>
#include <algorithm>

namespace std
{
    template <typename T>
    class vector
    {
    private:
        T* _data = nullptr;
        size_t _size = 0;
        size_t _capacity = 0;

    public:
        vector(size_t capacity)
        {
            resize(capacity);
        }

        vector(const vector<T>& old)
        {
            clear();

            for(int i = 0; i < old.size(); i++)
            {
                push_back(old[i]);
            }
        }

        class iterator
        {
        private:
            vector<T>& _vec;
            int _index;

        public:
            iterator(vector<T>& vec, int index)
            :
            _vec(vec),
            _index(index)
            {
            }

            iterator& operator++()
            {
                _index++;

                return *this;
            }

            iterator& operator--()
            {
                _index--;

                return *this;
            }

            iterator operator++(int)
            {
                iterator old = *this;

                operator++();

                return old;
            }

            iterator operator--(int)
            {
                iterator old = *this;

                operator--();

                return old;
            }

            bool operator==(const iterator& other)
            {
                return _index == other._index;
            }

            bool operator!=(const iterator& other)
            {
                return _index != other._index;
            }

            T& operator*()
            {
                return _vec.at(_index);
            }

            T* operator->()
            {
                return &_vec.at(_index);
            }
        };

        void operator=(const initializer_list<T>& l)
        {
            clear();

            for(const T& item : l)
            {
                push_back(item);
            }
        }

        vector(const initializer_list<T>& l)
        {
            operator=(l);
        }

        vector()
        {
            clear();
        }

        ~vector()
        {
            if(_data != nullptr)
            {
                delete[] _data;
            }
        }

        size_t capacity() const { return _capacity; }
        size_t size() const { return _size; }

        bool empty() const { return _size == 0; }

        T* data() { return _data; }
        const T* data() const { return _data; }

        T& at(size_t index) { return _data[index]; }
        const T& at(size_t index) const { return _data[index]; }

        T& operator[](int index)
        {
            return at(index);
        }

        const T& operator[](int index) const
        {
            return at(index);
        }

        void resize(size_t size)
        {
            if(size <= _capacity)
            {
                _size = size;
            }
            else
            {
                T* block = new T[size];

                if(_data)
                {
                    std::copy(_data, _data + _size, block);

                    delete[] _data;
                }

                _size = size;
                _capacity = size;
                _data = block;
            }
        }

        void push_back(const T& data)
        {
            resize(_size + 1);

            _data[_size - 1] = data;
        }

        void push_front(const T& data)
        {
            T* block = new T[_size + 1];

            if(_data != nullptr)
            {
                for(int i = 0; i < _size; i++)
                {
                    block[i + 1] = _data[i];
                }

                delete[] _data;
            }

            _capacity++;
            _size++;
            _data = block;
            _data[0] = data;
        }

        void clear()
        {
            resize(0);
        }

        iterator begin()
        {
            return iterator(*this, 0);
        }

        iterator end()
        {
            return iterator(*this, _size);
        }
    };
}
