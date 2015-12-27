#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stddef.h>
#include <stdint.h>

class Framebuffer
{
public:
    static Framebuffer init(size_t width, size_t height);
    
    bool valid() const { return _valid; }
    size_t width() const { return _valid ? _width : 0; }
    size_t height() const { return _valid ? _height : 0; }
    uint32_t *data() const { return _valid ? _data : nullptr; }
    
private:
    bool _valid;
    size_t _width;
    size_t _height;
    uint32_t *_data;
};

#endif
