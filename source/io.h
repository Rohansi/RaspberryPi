#ifndef IO_H
#define IO_H

enum
{
    PERIPHERAL_BASE = 0x3F000000,
    
    // The mailbox base address.
    MAIL_BASE       = PERIPHERAL_BASE + 0xB880,
    
    // The offsets for each mailbox register.
    MAIL_READ       = 0x00,
    MAIL_WRITE      = 0x20,
    MAIL_RSTATUS    = 0x18,
    MAIL_WSTATUS    = 0x38,
    
    // Masks for the mailbox status register.
    MAIL_EMPTY      = 0x40000000,
    MAIL_FULL       = 0x80000000,
    
    // Mailbox channels.
    MAIL_POWER      = 0,
    MAIL_FB         = 1,
    MAIL_VUART      = 2,
    MAIL_VCHIQ      = 3,
    MAIL_LED        = 4,
    MAIL_BUTTON     = 5,
    MAIL_TOUCH      = 6,
    MAIL_COUNT      = 7,
    MAIL_TAGS       = 8,
    
    // The GPIO registers base address.
    GPIO_BASE       = PERIPHERAL_BASE + 0x200000,

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD           = (GPIO_BASE + 0x94),

    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0       = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE      = GPIO_BASE + 0x1000,

    // The offsets for each register for the UART.
    UART0_DR        = (UART0_BASE + 0x00),
    UART0_RSRECR    = (UART0_BASE + 0x04),
    UART0_FR        = (UART0_BASE + 0x18),
    UART0_ILPR      = (UART0_BASE + 0x20),
    UART0_IBRD      = (UART0_BASE + 0x24),
    UART0_FBRD      = (UART0_BASE + 0x28),
    UART0_LCRH      = (UART0_BASE + 0x2C),
    UART0_CR        = (UART0_BASE + 0x30),
    UART0_IFLS      = (UART0_BASE + 0x34),
    UART0_IMSC      = (UART0_BASE + 0x38),
    UART0_RIS       = (UART0_BASE + 0x3C),
    UART0_MIS       = (UART0_BASE + 0x40),
    UART0_ICR       = (UART0_BASE + 0x44),
    UART0_DMACR     = (UART0_BASE + 0x48),
    UART0_ITCR      = (UART0_BASE + 0x80),
    UART0_ITIP      = (UART0_BASE + 0x84),
    UART0_ITOP      = (UART0_BASE + 0x88),
    UART0_TDR       = (UART0_BASE + 0x8C),
    
    // 
    V3D_BASE        = PERIPHERAL_BASE + 0xC00000,
    V3D_L2CACTL     = V3D_BASE + 0x00020,
    V3D_SLCACTL     = V3D_BASE + 0x00024,
    V3D_INTCTL      = V3D_BASE + 0x00030,
    V3D_INTENA      = V3D_BASE + 0x00034,
    V3D_INTDIS      = V3D_BASE + 0x00038,
    V3D_CT0CS       = V3D_BASE + 0x00100,
    V3D_CT1CS       = V3D_BASE + 0x00104,
    V3D_CT0EA       = V3D_BASE + 0x00108,
    V3D_CT1EA       = V3D_BASE + 0x0010C,
    V3D_CT0CA       = V3D_BASE + 0x00110,
    V3D_CT1CA       = V3D_BASE + 0x00114,
    V3D_BFC         = V3D_BASE + 0x00134,
    V3D_RFC         = V3D_BASE + 0x00138,
    V3D_BPOA        = V3D_BASE + 0x00308,
    V3D_BPOS        = V3D_BASE + 0x0030C,
    V3D_PCTRC       = V3D_BASE + 0x00670,
    V3D_PCTRE       = V3D_BASE + 0x00674,
    V3D_PCTR9       = V3D_BASE + 0x006C8,
    V3D_PCTR10      = V3D_BASE + 0x006D0,
    V3D_PCTR14      = V3D_BASE + 0x006F0,
    V3D_PCTR15      = V3D_BASE + 0x006F8,
    V3D_DBGE        = V3D_BASE + 0x00F00,
    V3D_FDBGO       = V3D_BASE + 0x00F04,
    V3D_FDBGB       = V3D_BASE + 0x00F08,
    V3D_FDBGR       = V3D_BASE + 0x00F0C,
    V3D_FDBGS       = V3D_BASE + 0x00F10,
    V3D_ERRSTAT     = V3D_BASE + 0x00F20,
};

inline void mmio_write(uint32_t reg, uint32_t data)
{
	*(volatile uint32_t *)reg = data;
}

inline uint32_t mmio_read(uint32_t reg)
{
	return *(volatile uint32_t *)reg;
}

inline void mailbox_write(uint8_t channel, uint32_t data)
{
    while ((mmio_read(MAIL_BASE + MAIL_WSTATUS) & MAIL_FULL) != 0)
    {
        // mailbox is full, wait
    }
    
    mmio_write(MAIL_BASE + MAIL_WRITE, data | channel);
}

#endif
