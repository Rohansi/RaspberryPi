#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include "framebuffer.h"
#include "buffer.h"
#include "control_list.h"

struct Character
{
    uint8_t foreground;
    uint8_t background;
    uint8_t glyph;
    uint8_t flags;
} __attribute__((packed));

static_assert(sizeof(Character) == sizeof(uint32_t), "Character struct is the wrong size");

struct TerminalUniforms
{
    size_t terminal_width;              // in characters
    const Character *terminal_data;     //
    const uint32_t *palette_data_fg;    // foreground palette colors, 256 max
    const uint32_t *palette_data_bg;    // background palette colors, 256 max
    const uint8_t *font_data;           // pointer to 8x12 RGBA font data, 256 characters, 16 per row
};

class Terminal
{
public:
    static Terminal init(const Framebuffer &framebuffer);
    void render(const Character *buffer);
    
private:
    Framebuffer _framebuffer;
    alignas(16) TerminalUniforms uniforms;
    alignas(16) NVShaderState shaderState;
    WriteBuffer<128, 16> binBuf;
    WriteBuffer<32 * 1024, 16> renBuf;
};

#endif
