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

bool Utils::parity(uint8_t input)
{
    uint8_t state = 0;

    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    input = input >> 1;
    state = state ^ (input & 1);

    return state;
}
