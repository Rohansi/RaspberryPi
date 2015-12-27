//
// Created by Daniel Lindberg on 2015-11-29.
//

#ifndef PRINTF_H
#define PRINTF_H

#include <stddef.h>
#include <stdarg.h>

int vsnprintf(char *buffer, size_t bufsz, const char *format, va_list arg);
int snprintf(char *buffer, size_t bufsz, const char *format, ...);

#endif
