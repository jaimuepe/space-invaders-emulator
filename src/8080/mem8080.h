#pragma once

#include <cstdint>

class Mem8080
{
public:

    void load();

    uint8_t &operator[](int idx);

    uint8_t operator[](int idx) const;

private:

    // 2^16 = 65536
    uint8_t *memory = new uint8_t[16 * 0x1000];
};
