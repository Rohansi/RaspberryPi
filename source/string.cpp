#include <stddef.h>
#include <stdint.h>

void *memcpy(void *destination, const void *source, size_t num) {
    uint8_t *d = (uint8_t*)destination;
    uint8_t *s = (uint8_t*)source;
    
    while (num-- != 0) {
        *d++ = *s++;
    }
    
    return destination;
}

void *memmove(void *destination, const void *source, size_t num) {
    uint8_t *d = (uint8_t*)destination;
    uint8_t *s = (uint8_t*)source;
    
    if (num == 0 || d == s)
        return destination;
    
    if (d < s) {
        // forward
        while (num-- != 0) {
            *d++ = *s++;
        }
    } else {
        // backward
        s += num;
        d += num;
        
        while (num-- != 0) {
            *d-- = *s--;
        }
    }
    
    return destination;
}

void *memset(void *ptr, int value, size_t num) {
    uint8_t *d = (uint8_t*)ptr;
    
    while (num-- != 0) {
        *d++ = value;
    }
    
    return ptr;
}

size_t strlen(const char *str) {
    size_t i = 0;
    
    while (*str++) {
        i++;
    }
    
    return i;
}
