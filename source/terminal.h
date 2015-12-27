#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include "framebuffer.h"

struct Character
{
    uint8_t foreground;
    uint8_t background;
    uint8_t glyph;
    uint8_t flags;
} __attribute__((packed));

static_assert(sizeof(Character) == sizeof(uint32_t), "Character struct is the wrong size");

class Terminal
{
public:
    static Terminal init(const Framebuffer &framebuffer);
    void render(const Character *buffer) const;
    
private:
    Framebuffer _framebuffer;
};

#endif
