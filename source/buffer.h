#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include "string.h"

template <size_t N, size_t A = 1>
class WriteBuffer
{
public:
    WriteBuffer()
    {
        _fail = false;
        _size = 0;
    }
    
    bool fail() const { return _fail; }
    size_t size() const { return _size; }
    void *data() { return &_data; }
    
    template <typename T>
    T *alloc()
    {
        if (_size + sizeof(T) >= N)
        {
            _fail = true;
            return nullptr;
        }
        
        uint8_t *ptr = &_data[_size];
        _size += sizeof(T);
        return reinterpret_cast<T*>(ptr);
    }
    
    template <typename T>
    T *write(const T &value)
    {
        T *ptr = alloc<T>();
        if (!ptr)
            return nullptr;
        
        //*ptr = value;
        memcpy(ptr, &value, sizeof(T));
        return ptr;
    }

private:
    bool _fail;
    size_t _size;
    alignas(A) uint8_t _data[N];
};

#endif
