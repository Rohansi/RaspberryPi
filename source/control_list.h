#ifndef CONTROL_LIST_H
#define CONTROL_LIST_H

#include <stddef.h>
#include <stdint.h>
#include "string.h"
#include "framebuffer.h"

// the structs in here shouldn't have any padding, which
// means we need to memcpy into fields when sizeof(T)>1

// TODO: dont use structs for this

enum class ControlOpCode : uint8_t
{
    halt                            = 0,
    nop                             = 1,
    flush                           = 4,
    flush_all_state                 = 5,
    start_tile_binning              = 6,
    increment_semaphore             = 7,
    wait_on_semaphore               = 8,
    branch                          = 16,
    branch_to_sublist               = 17,
    return_from_sublist             = 18,
    store_multisample               = 24,
    store_multisample_end           = 25,
    store_full_tile_buffer          = 26,
    reload_full_tile_buffer         = 27,
    store_tile_buffer               = 28,
    load_tile_buffer                = 29,
    indexed_primitive_list          = 32,
    vertex_array_primitives         = 33,
    vg_coordinate_array_primitives  = 41,
    compressed_primitive_list       = 48,
    clip_compressed_primitive_list  = 49,
    primitive_list_format           = 56,
    gl_shader_state                 = 64,
    nv_shader_state                 = 65,
    vg_shader_state                 = 66,
    vg_inline_shader_record         = 67,
    config_bits                     = 96,
    flat_shade_flags                = 97,
    points_size                     = 98,
    line_width                      = 99,
    rht_x_boundary                  = 100,
    depth_offset                    = 101,
    clip_window                     = 102,
    viewport_offset                 = 103,
    z_clipping_planes               = 104,
    clipper_xy_scaling              = 105,
    clipper_z_scale_offset          = 106, // 105 in documentation, but it's where 106 should be
    tile_binning_config             = 112,
    tile_rendering_config           = 113,
    clear_colors                    = 114,
    tile_coordinates                = 115
};

enum class PrimitiveMode : uint8_t
{
    points                          = 0,
    lines                           = 1,
    line_loop                       = 2,
    line_strip                      = 3,
    triangles                       = 4,
    triangle_strip                  = 5,
    triangle_fan                    = 6
};

enum class NVShaderFlags : uint8_t
{
    // TODO: not sure if these are actually flags
    none                            = 0,
    single_threaded                 = 1,
    has_point_size                  = 2,
    enable_clipping                 = 4,
    clip_coordinates_header         = 8
};

struct NVShaderState
{
    NVShaderFlags flags;
    uint8_t vertex_stride;
    uint8_t uniform_count;
    uint8_t varying_count;
    void *fragment_shader;
    void *fragment_shader_uniforms;
    void *vertex_data;
    
    NVShaderState()
    {
        // TODO: need to memcpy these
        this->flags = NVShaderFlags::single_threaded;
        this->vertex_stride = 0;
        this->uniform_count = 0;
        this->varying_count = 0;
        this->fragment_shader = nullptr;
        this->fragment_shader_uniforms = nullptr;
        this->vertex_data = nullptr;
    }
} __attribute__((packed));

struct ControlEntry
{
    ControlOpCode opcode;
    
    ControlEntry(ControlOpCode opcode)
    {
        this->opcode = opcode;
    }
} __attribute__((packed));

struct ControlHalt : public ControlEntry
{
    ControlHalt()
        : ControlEntry(ControlOpCode::halt)
    {
        
    }
} __attribute__((packed));

struct ControlNop : public ControlEntry
{
    ControlNop()
        : ControlEntry(ControlOpCode::nop)
    {
        
    }
} __attribute__((packed));

struct ControlFlush : public ControlEntry
{
    ControlFlush()
        : ControlEntry(ControlOpCode::flush)
    {
        
    }
} __attribute__((packed));

struct ControlFlushAllState : public ControlEntry
{
    ControlFlushAllState()
        : ControlEntry(ControlOpCode::flush_all_state)
    {
        
    }
} __attribute__((packed));

struct ControlStartTileBinning : public ControlEntry
{
    ControlStartTileBinning()
        : ControlEntry(ControlOpCode::start_tile_binning)
    {
        
    }
} __attribute__((packed));

struct ControlBranchToSublist : public ControlEntry
{
    void *address;
    
    ControlBranchToSublist(void *address)
        : ControlEntry(ControlOpCode::branch_to_sublist)
    {
        memcpy(&this->address, &address, sizeof(address));
    }
} __attribute__((packed));

static_assert(sizeof(ControlBranchToSublist) == (sizeof(uint8_t) + sizeof(void*)), "ControlEntry padding");

struct ControlStoreMultisample : public ControlEntry
{
    ControlStoreMultisample()
        : ControlEntry(ControlOpCode::store_multisample)
    {
        
    }
} __attribute__((packed));

struct ControlStoreMultisampleEnd : public ControlEntry
{
    ControlStoreMultisampleEnd()
        : ControlEntry(ControlOpCode::store_multisample_end)
    {
        
    }
} __attribute__((packed));

struct ControlStoreTileBuffer : public ControlEntry
{
    uint16_t data16;
    uint32_t data32;
    
    ControlStoreTileBuffer()
        : ControlEntry(ControlOpCode::store_tile_buffer)
    {
        // TODO: fields
        memset(&this->data16, 0, sizeof(this->data16));
        memset(&this->data32, 0, sizeof(this->data32));
    }
} __attribute__((packed));

struct ControlVertexArrayPrimitives : public ControlEntry
{
    PrimitiveMode primitive_mode;
    size_t length;
    size_t offset;
    
    ControlVertexArrayPrimitives(PrimitiveMode primitive_mode, size_t offset, size_t length)
        : ControlEntry(ControlOpCode::vertex_array_primitives)
    {
        memcpy(&this->primitive_mode, &primitive_mode, sizeof(primitive_mode));
        memcpy(&this->length, &length, sizeof(length));
        memcpy(&this->offset, &offset, sizeof(offset));
    }
} __attribute__((packed));

struct ControlNVShaderState : public ControlEntry
{
    NVShaderState *ptr;
    
    ControlNVShaderState(NVShaderState *ptr)
        : ControlEntry(ControlOpCode::nv_shader_state)
    {
        memcpy(&this->ptr, &ptr, sizeof(ptr));
    }
} __attribute__((packed));

enum ControlConfigBitsFlags : uint32_t
{
    forward_facing              = 0x000001,
    reverse_facing              = 0x000002,
    clockwise_primitives        = 0x000004,
    depth_offset                = 0x000008,
    antialiased_points_lines    = 0x000010, // not supported
    coverage_read_type          = 0x000020,
    rasterizer_oversample_none  = 0x000000, // 0
    rasterizer_oversample_4x    = 0x000040, // 1
    rasterizer_oversample_16x   = 0x000080, // 2
    rasterizer_oversample_resv  = 0x0000C0, // 3
    coverage_pipe_select        = 0x000100,
    coverage_update_mode_nz     = 0x000000, // 0
    coverage_update_mode_odd    = 0x000200, // 1
    coverage_update_mode_or     = 0x000400, // 2
    coverage_update_mode_z      = 0x000600, // 3
    coverage_read_mode          = 0x000800,
    depth_test_function_never   = 0x000000, // 0
    depth_test_function_lt      = 0x001000, // 1
    depth_test_function_eq      = 0x002000, // 2
    depth_test_function_le      = 0x003000, // 3
    depth_test_function_gt      = 0x004000, // 4
    depth_test_function_ne      = 0x005000, // 5
    depth_test_function_ge      = 0x006000, // 6
    depth_test_function_always  = 0x007000, // 7
    z_updates                   = 0x008000,
    early_z                     = 0x010000,
    early_z_updates             = 0x020000
};

struct ControlConfigBits : public ControlEntry
{
    uint8_t data8_0;
    uint8_t data8_1;
    uint8_t data8_2;
    
    ControlConfigBits(ControlConfigBitsFlags flags)
        : ControlEntry(ControlOpCode::config_bits)
    {
        uint32_t value = (uint32_t)flags;
        
        data8_0 = value & 0xFF;
        data8_1 = (value >> 8) & 0xFF;
        data8_2 = (value >> 16) & 0xFF;
    }
} __attribute__((packed));

struct ControlClipWindow : public ControlEntry
{
    uint16_t left;
    uint16_t bottom;
    uint16_t width;
    uint16_t height;
    
    ControlClipWindow(uint16_t left, uint16_t bottom, uint16_t width, uint16_t height)
        : ControlEntry(ControlOpCode::clip_window)
    {
        memcpy(&this->left, &left, sizeof(left));
        memcpy(&this->bottom, &bottom, sizeof(bottom));
        memcpy(&this->width, &width, sizeof(width));
        memcpy(&this->height, &height, sizeof(height));
    }
} __attribute__((packed));

struct ControlViewportOffset : public ControlEntry
{
    uint16_t x;
    uint16_t y;
    
    ControlViewportOffset(uint16_t x, uint16_t y)
        : ControlEntry(ControlOpCode::viewport_offset)
    {
        memcpy(&this->x, &x, sizeof(x));
        memcpy(&this->y, &y, sizeof(y));
    }
} __attribute__((packed));

enum class ControlTileBinningConfigFlags : uint8_t
{
    multisample_mode        = 0b0000'0001,
    color_depth_64          = 0b0000'0010,
    auto_init_state         = 0b0000'0100,
    tile_alloc_initial_32   = 0b0000'0000, // 0
    tile_alloc_initial_64   = 0b0000'1000, // 1
    tile_alloc_initial_128  = 0b0001'0000, // 2
    tile_alloc_initial_256  = 0b0001'1000, // 3
    tile_alloc_block_32     = 0b0000'0000, // 0
    tile_alloc_block_64     = 0b0010'0000, // 1
    tile_alloc_block_128    = 0b0100'0000, // 2
    tile_alloc_block_256    = 0b0110'0000, // 3
    double_buffer           = 0b1000'0000
};

struct ControlTileBinningConfig : public ControlEntry
{
    void *tile_buffer;
    size_t tile_buffer_size;
    void *tile_state_buffer;
    uint8_t width; // in tiles
    uint8_t height; // in tiles
    ControlTileBinningConfigFlags flags;
    
    ControlTileBinningConfig(
        void *tile_buffer,
        size_t tile_buffer_size,
        void *tile_state_buffer,
        uint8_t width,
        uint8_t height,
        ControlTileBinningConfigFlags flags)
        : ControlEntry(ControlOpCode::tile_binning_config)
    {
        memcpy(&this->tile_buffer, &tile_buffer, sizeof(tile_buffer));
        memcpy(&this->tile_buffer_size, &tile_buffer_size, sizeof(tile_buffer_size));
        memcpy(&this->tile_state_buffer, &tile_state_buffer, sizeof(tile_state_buffer));
        
        this->width = width;
        this->height = height;
        this->flags = flags;
    }
} __attribute__((packed));

enum class ControlTileRenderingConfigFlags : uint16_t
{
    multisample_mode        = 0x0001,
    color_depth_64          = 0x0002,
    color_format_bgr565d    = 0x0000, // 0
    color_format_rgba8888   = 0x0004, // 1
    color_format_bgr565     = 0x0008, // 2
    decimate_mode_1x        = 0x0000, // 0
    decimate_mode_4x        = 0x0010, // 1
    decimate_mode_16x       = 0x0020, // 2
    memory_format_linear    = 0x0000, // 0
    memory_format_t         = 0x0040, // 1
    memory_format_lt        = 0x0080, // 2
    enable_vg_mask          = 0x0100,
    select_coverage_mode    = 0x0200,
    early_z_update_dir      = 0x0400,
    early_z_cov_disable     = 0x0800,
    double_buffer           = 0x1000
};

struct ControlTileRenderingConfig : public ControlEntry
{
    void *ptr;
    uint16_t width;
    uint16_t height;
    ControlTileRenderingConfigFlags flags;
    
    ControlTileRenderingConfig(const Framebuffer &framebuffer, ControlTileRenderingConfigFlags flags)
        : ControlEntry(ControlOpCode::tile_rendering_config)
    {
        void *fbPtr = framebuffer.data();
        memcpy(&this->ptr, &fbPtr, sizeof(fbPtr));
        
        uint16_t width = framebuffer.width();
        memcpy(&this->width, &width, sizeof(width));
        
        uint16_t height = framebuffer.height();
        memcpy(&this->height, &height, sizeof(height));
        
        memcpy(&this->flags, &flags, sizeof(flags));
    }
} __attribute__((packed));

struct ControlClearColors : public ControlEntry
{
    uint32_t clear_color1;
    uint32_t clear_color2;
    uint32_t clear_z;
    uint8_t clear_stencil;
    
    ControlClearColors(uint32_t clear_color)
        : ControlEntry(ControlOpCode::clear_colors)
    {
        memcpy(&this->clear_color1, &clear_color, sizeof(clear_color));
        memcpy(&this->clear_color2, &clear_color, sizeof(clear_color));
        
        // TODO: fields
        memset(&this->clear_z, 0, sizeof(this->clear_z));
        this->clear_stencil = 0;
    }
} __attribute__((packed));

struct ControlTileCoordinates : public ControlEntry
{
    uint8_t x;
    uint8_t y;
    
    ControlTileCoordinates(uint8_t x, uint8_t y)
        : ControlEntry(ControlOpCode::tile_coordinates)
    {
        this->x = x;
        this->y = y;
    }
} __attribute__((packed));

#endif
