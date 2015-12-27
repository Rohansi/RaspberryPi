#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <stddef.h>
#include <stdint.h>

class Frequency
{
public:
    Frequency()
    {
        _hertz = 0;
    }
    
    explicit constexpr Frequency(uint32_t hertz)
        : _hertz(hertz)
    {
        
    }
    
private:
    uint32_t _hertz;
};

static_assert(sizeof(Frequency) == sizeof(uint32_t), "Frequency class is the wrong size");

constexpr Frequency operator"" _Hz(unsigned long long hz)
{
    return Frequency(hz);
}

constexpr Frequency operator"" _kHz(unsigned long long khz)
{
    return Frequency(khz * 1000);
}

constexpr Frequency operator"" _mHz(unsigned long long mhz)
{
    return Frequency(mhz * 1000 * 1000);
}

#endif
