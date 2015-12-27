#include <stddef.h>
#include <stdint.h>
#include "uart.h"

/* list of possible tags */
#define ATAG_NONE       0x00000000
#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009

/* structures for each atag */
struct atag_header {
        uint32_t size; /* length of tag in words including this header */
        uint32_t tag;  /* tag type */
};

struct atag_core {
        uint32_t flags;
        uint32_t pagesize;
        uint32_t rootdev;
};

struct atag_mem {
        uint32_t     size;
        uint32_t     start;
};

struct atag_videotext {
        uint8_t              x;
        uint8_t              y;
        uint16_t             video_page;
        uint8_t              video_mode;
        uint8_t              video_cols;
        uint16_t             video_ega_bx;
        uint8_t              video_lines;
        uint8_t              video_isvga;
        uint16_t             video_points;
};

struct atag_ramdisk {
        uint32_t flags;
        uint32_t size;
        uint32_t start;
};

struct atag_initrd2 {
        uint32_t start;
        uint32_t size;
};

struct atag_serialnr {
        uint32_t low;
        uint32_t high;
};

struct atag_revision {
        uint32_t rev;
};

struct atag_videolfb {
        uint16_t             lfb_width;
        uint16_t             lfb_height;
        uint16_t             lfb_depth;
        uint16_t             lfb_linelength;
        uint16_t             lfb_base;
        uint16_t             lfb_size;
        uint8_t              red_size;
        uint8_t              red_pos;
        uint8_t              green_size;
        uint8_t              green_pos;
        uint8_t              blue_size;
        uint8_t              blue_pos;
        uint8_t              rsvd_size;
        uint8_t              rsvd_pos;
};

struct atag_cmdline {
        char    cmdline[1];
};

struct atag {
        struct atag_header hdr;
        union {
                struct atag_core         core;
                struct atag_mem          mem;
                struct atag_videotext    videotext;
                struct atag_ramdisk      ramdisk;
                struct atag_initrd2      initrd2;
                struct atag_serialnr     serialnr;
                struct atag_revision     revision;
                struct atag_videolfb     videolfb;
                struct atag_cmdline      cmdline;
        } u;
};

void dump_atags(void *ptr)
{
    atag *tags = (atag*)ptr;
    uart_printf("ATAGS at %08X\r\n", tags);
    
    while (tags->hdr.tag != ATAG_NONE)
    {
        switch (tags->hdr.tag)
        {
            case ATAG_CORE:
                uart_printf("ATAG_CORE\r\n");
                uart_printf("  flags    = %08X\r\n", tags->u.core.flags);
                uart_printf("  pagesize = %08X\r\n", tags->u.core.pagesize);
                uart_printf("  rootdev  = %08X\r\n", tags->u.core.rootdev);
                break;
            
            case ATAG_MEM:
                uart_printf("ATAG_MEM\r\n");
                uart_printf("  size     = %08X\r\n", tags->u.mem.size);
                uart_printf("  start    = %08X\r\n", tags->u.mem.start);
                break;
            
            case ATAG_VIDEOTEXT:
                uart_printf("ATAG_VIDEOTEXT\r\n");
                break;
            
            case ATAG_RAMDISK:
                uart_printf("ATAG_RAMDISK\r\n");
                break;
            
            case ATAG_INITRD2:
                uart_printf("ATAG_INITRD2\r\n");
                break;
            
            case ATAG_SERIAL:
                uart_printf("ATAG_SERIAL\r\n");
                break;
            
            case ATAG_REVISION:
                uart_printf("ATAG_REVISION\r\n");
                break;
            
            case ATAG_VIDEOLFB:
                uart_printf("ATAG_VIDEOLFB\r\n");
                break;
            
            case ATAG_CMDLINE:
                uart_printf("ATAG_CMDLINE\r\n");
                uart_printf("  %s\r\n", &tags->u.cmdline.cmdline);
                break;
            
            default:
                uart_printf("unknown\r\n");
                break;
        }
        
        tags = (atag*)((uint32_t*)tags + tags->hdr.size);
    }
}
