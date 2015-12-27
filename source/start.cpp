#include <stddef.h>
#include <stdint.h>

typedef void (*InitFunc)();

extern uint32_t __init_array_start;
extern uint32_t __init_array_end;

extern "C"
void _init_array()
{
    // Calls constructors for global variables
    for (auto *p = &__init_array_start; p < &__init_array_end; ++p)
    {
        auto f = (InitFunc)*p;
        f();
    }
}
