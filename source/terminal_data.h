#include "terminal_font.h"

struct Vertex {
    int16_t x; // 12.4 fixed point
    int16_t y; // 12.4 fixed point
    float z;
    float w;
} __attribute__((packed));

alignas(16) static Vertex vertices[] = {
    { 32   << 4, 32   << 4, 1, 1 }, // top left
    { 32   << 4, 1048 << 4, 1, 1 }, // bottom left
    { 1888 << 4, 32   << 4, 1, 1 }, // top right
    { 1888 << 4, 1048 << 4, 1, 1 }  // bottom right
};

/*alignas(16) static Vertex vertices[] = {
    { 0    << 4, 0    << 4, 1, 1 }, // top left
    { 0    << 4, 1080 << 4, 1, 1 }, // bottom left
    { 1920 << 4, 0    << 4, 1, 1 }, // top right
    { 1920 << 4, 1080 << 4, 1, 1 }  // bottom right
};*/

alignas(16) static uint32_t palette[] = {
    0xFF000000, //   0
    0xFF0000A8, //   1
    0xFF00A800, //   2
    0xFF00A8A8, //   3
    0xFFA80000, //   4
    0xFFA800A8, //   5
    0xFFA85400, //   6
    0xFFA8A8A8, //   7
    0xFF545454, //   8
    0xFF5454FC, //   9
    0xFF54FC54, //  10
    0xFF54FCFC, //  11
    0xFFFC5454, //  12
    0xFFFC54FC, //  13
    0xFFFCFC54, //  14
    0xFFFFFFFF, //  15
};

alignas(16) static uint32_t shader_code[] = {
    #include "texter-simple.qpu.inc"
};
