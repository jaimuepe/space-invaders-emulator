#pragma once

#include <cstdint>
#include <string>

class Utils
{
public:
    static std::string to_hex_string(uint8_t input);

    static std::string to_hex_string(uint16_t input);

    static uint16_t to_16(uint8_t lo, uint8_t hi);

    static bool parity(uint8_t input);
};