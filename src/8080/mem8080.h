#pragma once

#include <cstdint>

class Mem8080
{
public:
    void init();

    const uint8_t &operator[](uint16_t pos) const;

    uint8_t &operator[](uint16_t pos);

private:
    // 2^16 = 65536
    uint8_t *memory = new uint8_t[16 * 0x1000];
};
