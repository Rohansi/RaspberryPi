#ifndef UART_H
#define UART_H

void uart_init();
void uart_putc(char byte);
char uart_getc();
void uart_write(const char *buffer, size_t size);
void uart_puts(const char *str);
void uart_printf(const char *format, ...);

#endif
