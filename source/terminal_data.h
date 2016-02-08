#include "terminal_font.h"
#include "terminal_palette.h"

struct Vertex {
    int16_t x; // 12.4 fixed point
    int16_t y; // 12.4 fixed point
    float z;
    float w;
} __attribute__((packed));

/*alignas(16) static Vertex vertices[] = {
    { 32   << 4, 32   << 4, 1, 1 }, // top left
    { 32   << 4, 1048 << 4, 1, 1 }, // bottom left
    { 1888 << 4, 32   << 4, 1, 1 }, // top right
    { 1888 << 4, 1048 << 4, 1, 1 }  // bottom right
};*/

alignas(16) static Vertex vertices[] = {
    { 0    << 4, 0    << 4, 1, 1 }, // top left
    { 0    << 4, 1080 << 4, 1, 1 }, // bottom left
    { 1920 << 4, 0    << 4, 1, 1 }, // top right
    { 1920 << 4, 1080 << 4, 1, 1 }  // bottom right
};

alignas(16) static uint32_t shader_code[] = {
    #include "texter.qpu.inc"
};
