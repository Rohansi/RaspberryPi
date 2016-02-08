#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "uart.h"
#include "atag.h"
#include "framebuffer.h"
#include "terminal.h"
#include "io.h"

// https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=98904#p691475
// https://www.raspberrypi.org/forums/viewtopic.php?f=72&t=98904&start=25#p703272
// https://github.com/mrvn/test/blob/master/mmu.cc

extern "C" void _start1();
extern "C" void _start2();
extern "C" void _start3();

extern "C"
void kmain(uint32_t r0, uint32_t r1, uint32_t atags)
{
	(void) r0;
	(void) r1;
	(void) atags;
    
    //asm volatile("ldr r3, =0x3F200000; mov r4, #1; lsl r4, #15; str r4, [r3,#0x20]" ::: "r3", "r4");
    
	uart_init();
	uart_puts("hello!!\r\n");
    
    auto atagsPtr = (void*)0x100; // TODO: why is atags arg corrupted? probably U-Boot
    dump_atags(atagsPtr);
    
    // wake up other cores so they don't slow down everything
    mmio_write(0x4000008C + 0x10 * 1, (uint32_t)&_start1);
    mmio_write(0x4000008C + 0x10 * 2, (uint32_t)&_start2);
    mmio_write(0x4000008C + 0x10 * 3, (uint32_t)&_start3);
    asm volatile("dmb");
    
    auto framebuffer = Framebuffer::init(1920, 1080);
    if (framebuffer.valid())
    {
        uart_printf("fb=%.8X\r\n", framebuffer.data());
        
        auto term = Terminal::init(framebuffer);
        
        size_t width = framebuffer.width() / 8;
        size_t height = framebuffer.height() / 12;
        size_t length = width * height;
        
        Character *buf = (Character*)0x1000000;
        for (size_t i = 0; i < length; i++)
        {
            buf[i] = { 0, 0, 0, 0 };
        }
        
        size_t t1 = 0, t2 = 0;
        while (true)
        {
            size_t i = 0;
            for (size_t y = 0; y < height; y++)
            {
                for (size_t x = 0; x < width; x++, i++)
                {
                    uint8_t col = (uint8_t)(t2 + i);
                    uint8_t ch = (uint8_t)((t1 + x) ^ (t1 + y));
                    buf[i] = { (uint8_t)-col, col, ch, 0 };
                }
            }
            
            term.render(buf);
            t1++;
            t2--;
        }
    }
    
	while (true)
		uart_putc(uart_getc());
}

extern "C"
void core1()
{
    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C"
void core2()
{
    while (true)
    {
        asm volatile("wfe");
    }
}

extern "C"
void core3()
{
    while (true)
    {
        asm volatile("wfe");
    }
}
