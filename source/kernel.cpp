#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "uart.h"
#include "atag.h"
#include "framebuffer.h"
#include "terminal.h"
#include "io.h"

// https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=98904#p691475

extern "C"
void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
	(void) r0;
	(void) r1;
	(void) atags;
    
    //asm volatile("ldr r3, =0x3F200000; mov r4, #1; lsl r4, #15; str r4, [r3,#0x20]" ::: "r3", "r4");
    
	uart_init();
	uart_puts("hello!!\r\n");
    
    auto atagsPtr = (void*)0x100; // TODO: why is atags arg corrupted?
    dump_atags(atagsPtr);
    
    auto framebuffer = Framebuffer::init(1920, 1080);
    if (framebuffer.valid())
    {
        auto term = Terminal::init(framebuffer);
        
        //size_t cx = framebuffer.width() / 8;
        //size_t cy = framebuffer.height() / 12;
        //size_t cn = cx * cy;
        
        //Character *buf = (Character*)0x1000000;
        uint32_t *buf = (uint32_t*)0x1000000;
        
        //size_t g = 3;
        uint8_t c = 0;
        
        while (true)
        {
            // clear caches
            mmio_write(V3D_L2CACTL, 4);
            mmio_write(V3D_SLCACTL, 0x0F0F0F0F);
            asm volatile("dmb");
            
            /*for (size_t i = 0; i < cn; i++)
            {
                buf[i] = { (uint8_t)(1 + c % 15), 0, (uint8_t)g, 0 };
            }*/
            
            static uint32_t colors[] = {
                0xFFFFFFFF,
                0xFFFF0000,
                0xFF00FF00,
                0xFF0000FF,
                0xFF000000
            };
            
            uint32_t col = colors[c % 5];
            for (size_t i = 0; i < 128*128; i++)
            {
                buf[i] = col;
            }
            
            /*for (size_t y = 0; y < 128; y++)
            {
                for (size_t x = 0; x < 128; x++)
                {
                    buf[y * 128 + x] = (c + x * 2) ^ (c + y * 2);
                }
            }*/
            
            term.render((Character*)buf);
            
            uart_getc();
            c++;
        }
    }
    
	while (true)
		uart_putc(uart_getc());
}
