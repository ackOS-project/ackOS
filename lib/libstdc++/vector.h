#pragma once
#include <stdlib.h>
//#include "../stdio.h"

namespace std
{

  template <typename T>
  class vector
  {
  private:
    T *m_Array;
    size_t m_Capacity = 0;
    size_t m_Items = 0;

  public:
    void MakeArray(size_t size)
    {
      m_Capacity = size;
      m_Items -= 1;
      m_Array = (T *)malloc(size * sizeof(T));
    }
    vector(size_t size)
    {
      MakeArray(size);
    }
    vector()
    {
      MakeArray(1);
    }
    ~vector()
    {
      free(m_Array);
    }
    size_t size()
    {
      return m_Capacity;
    }
    T *data() { return m_Array; }
    T GetAt(int index) { return m_Array[index]; }
    void SetAt(int index, T data)
    {
      if (index >= m_Capacity)
      {
        int diff = (abs(m_Capacity - index) + 1);
        T *tmp = (T *)malloc((m_Capacity + diff) * sizeof(T));
        for (int i = 0; i < m_Capacity; i++)
        {
          tmp[i] = m_Array[i];
        };
        free(m_Array);
        m_Array = tmp;
        m_Capacity += diff;
      }
      m_Items += 1;
      m_Array[index] = data;
    }
    T operator[](int index) { return GetAt(index); }
    vector push_back(T data)
    {
      SetAt(m_Items + 1, data);
      return *this;
    }
    vector operator+=(T data) { push_back(data); }
  };

} // namespace std