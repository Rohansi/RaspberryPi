#ifndef MAILBOX_TAGS_H
#define MAILBOX_TAGS_H

#include "frequency.h"

// https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface#tags-arm-to-vc

struct Tag
{
    uint32_t type;
    uint32_t size;
    uint32_t length;
    
    Tag(uint32_t type, uint32_t size)
    {
        size -= sizeof(Tag);
        
        this->type = type;
        this->size = size;
        this->length = size;
    }
};

struct TagEnd
{
    uint32_t type;
    
    TagEnd()
    {
        this->type = 0;
    }
};

enum class ClockID : uint32_t
{
    emmc    = 0x1,
    uart    = 0x2,
    arm     = 0x3,
    core    = 0x4,
    v3d     = 0x5,
    h264    = 0x6,
    isp     = 0x7,
    sdram   = 0x8,
    pixel   = 0x9,
    pwm     = 0xA
};

struct TagSetClockRate : public Tag
{
    ClockID id;
    Frequency value;
    
    TagSetClockRate(ClockID id, Frequency value)
        : Tag(0x00038002, sizeof(TagSetClockRate))
    {
        this->id = id;
        this->value = value;
    }
};

struct TagEnableQPU : public Tag
{
    uint32_t enabled;
    
    TagEnableQPU(bool enabled)
        : Tag(0x00030012, sizeof(TagEnableQPU))
    {
        this->enabled = enabled;
    }
};

struct TagSetPhysicalDisplay : public Tag
{
    uint32_t width;
    uint32_t height;
    
    TagSetPhysicalDisplay(uint32_t width, uint32_t height)
        : Tag(0x00048003, sizeof(TagSetPhysicalDisplay))
    {
        this->width = width;
        this->height = height;
    }
};

struct TagSetVirtualBuffer : public Tag
{
    uint32_t width;
    uint32_t height;
    
    TagSetVirtualBuffer(uint32_t width, uint32_t height)
        : Tag(0x00048004, sizeof(TagSetVirtualBuffer))
    {
        this->width = width;
        this->height = height;
    }
};

struct TagSetDepth : public Tag
{
    uint32_t bpp;
    
    TagSetDepth(uint32_t bpp)
        : Tag(0x00048005, sizeof(TagSetDepth))
    {
        this->bpp = bpp;
    }
};

struct TagSetVirtualOffset : public Tag
{
    uint32_t x;
    uint32_t y;
    
    TagSetVirtualOffset(uint32_t x, uint32_t y)
        : Tag(0x00048009, sizeof(TagSetVirtualOffset))
    {
        this->x = x;
        this->y = y;
    }
};

struct TagAllocateBuffer : public Tag
{
    void *ptr;
    uint32_t size;
    
    TagAllocateBuffer()
        : Tag(0x00040001, sizeof(TagAllocateBuffer))
    {
        this->ptr = 0;
        this->size = 0;
    }
};

#endif
