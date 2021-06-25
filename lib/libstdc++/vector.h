#pragma once
#include <stdlib.h>
//#include "../stdio.h"

namespace std
{

  template <typename T>
  class vector
  {
  private:
    T *m_array = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

  public:
    vector(size_t capacity)
    {
        m_size = 0;
        m_capacity = capacity;
        m_array = new T[m_capacity];
    }

    vector()
    {
        vector(2);
    }

    ~vector()
    {
        free((void*)m_array);
    }

    size_t capacity()
    {
        return m_capacity;
    }

    size_t size()
    {
        return m_size;
    }

    T *data()
    {
        return m_array;
    }

    T& at(size_t index) { return m_array[index]; }

    T operator[](int index)
    {
        return at(index);
    }

    void resize(size_t count)
    {
        T* block = new T[count];

        if(m_capacity < m_size)
        {
            m_size = count;
        }

        for(size_t i; i < m_size; i++)
        {
            block[i] = m_array[i];
        }

        m_capacity = count;
    }

    vector& push_back(T data)
    {
        resize(m_capacity + 1);
        at(m_size + 1) = data;

        return *this;
    }

    vector operator+=(T data)
    {
        push_back(data);
    }
  };

}