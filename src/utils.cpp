#include <cstdint>
#include <format>

#include "utils.h"

std::string Utils::to_hex_string(uint8_t input)
{
    return std::format("{:02X}", input);
}

std::string Utils::to_hex_string(uint16_t input)
{
    return std::format("{:04X}", input);
}

uint16_t Utils::to_addressLH(uint8_t lo, uint8_t hi)
{
    return lo | hi << 8;
}

bool Utils::parity(uint8_t x)
{
    int size = 8;

    int i;
    int p = 0;
    x = (x & ((1 << size) - 1));
    for (i = 0; i < size; i++)
    {
        if (x & 0x1)
            p++;
        x = x >> 1;
    }
    return (0 == (p & 0x1));
}
