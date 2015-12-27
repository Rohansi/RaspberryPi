#include "framebuffer.h"
#include "io.h"
#include "buffer.h"
#include "mailbox_tags.h"
#include "uart.h"

typedef WriteBuffer<128, 16> TagBuffer;

static bool check_response(void *ptr);

Framebuffer Framebuffer::init(size_t width, size_t height)
{
    Framebuffer result;
    
    {
        // Initialize V3D
        TagBuffer buf;
        
        uint32_t *size = buf.alloc<uint32_t>();
        volatile uint32_t *req  = buf.write<uint32_t>(0);
        
        buf.write(TagSetClockRate(ClockID::v3d, 250_mHz));
        buf.write(TagEnableQPU(true));
        buf.write(TagEnd());
        
        if (buf.fail())
            goto fail;
        
        *size = buf.size();
        
        mailbox_write(MAIL_TAGS, (uint32_t)buf.data());
        while (*req == 0) { } // wait for completion
        
        if (!check_response(buf.data()))
            goto fail;
    }
    
    {
        // Initialize framebuffer
        TagBuffer buf;
        
        uint32_t *size = buf.alloc<uint32_t>();
        volatile uint32_t *req  = buf.write<uint32_t>(0);
        
        auto phys = buf.write(TagSetPhysicalDisplay(width, height));
        auto virt = buf.write(TagSetVirtualBuffer(width, height));
        auto depth = buf.write(TagSetDepth(32));
        auto offs = buf.write(TagSetVirtualOffset(0, 0));
        auto fb = buf.write(TagAllocateBuffer());
        buf.write(TagEnd());
        
        if (buf.fail())
            goto fail;
        
        *size = buf.size();
        
        mailbox_write(MAIL_TAGS, (uint32_t)buf.data());
        while (*req == 0) { } // wait for completion
        
        if (!check_response(buf.data()))
            goto fail;
        
        // make sure the display parameters were set correctly
        if (phys->width != width || phys->height != height)
            goto fail;
        
        if (virt->width != width || virt->height != height)
            goto fail;
        
        if (depth->bpp != 32)
            goto fail;
        
        if (offs->x != 0 || offs->y != 0)
            goto fail;
        
        // convert address from VC to ARM
        uint32_t ptr = (uint32_t)fb->ptr & 0x3FFFFFFF;
        if (ptr == 0)
            goto fail;
            
        result._data = (uint32_t*)ptr;
    }
    
    result._valid = true;
    result._width = width;
    result._height = height;
    return result;
    
fail:
    result._valid = false;
    return result;
}

static bool check_response(void *ptr)
{
    uint32_t *req = (uint32_t*)ptr + 1;
    uint32_t *data = (uint32_t*)ptr + 2;
    
    if (*req == 0)
    {
        uart_puts("No response?\r\n");
        return false;
    }
    else if (*req == 0x80000001)
    {
        uart_puts("Partial success:\r\n");
        
        while (data[0])
        {
            uint32_t size = (data[1] / sizeof(uint32_t)) + 3;
            uart_printf("[%08X %2d] %08X -> %d\r\n", data, size, data[0], data[2]);
            
            data += size;
        }
        
        return false;
    }
    
    return true;
}
