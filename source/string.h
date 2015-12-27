#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *destination, const void *source, size_t num);
void *memmove(void *destination, const void *source, size_t num);
void *memset(void *ptr, int value, size_t num);
size_t strlen(const char *str);

#endif
