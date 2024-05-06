#pragma once

#include <cstdint>
#include <ostream>

struct State
{
public:
    State();

    uint8_t a{0};
    uint8_t b{0};
    uint8_t c{0};
    uint8_t d{0};
    uint8_t e{0};
    uint8_t h{0};
    uint8_t l{0};

    uint16_t sp{0};
    uint16_t pc{0};

    void set_mem(uint16_t addr, uint8_t value);

    uint8_t get_mem(uint16_t addr) const;

    struct Flags
    {
        bool s;
        bool z;
        bool p;
        bool c;
    } flags;

    friend std::ostream &operator<<(std::ostream &stream, const State &state);

private:
    // 2^16 = 65536
    uint8_t *memory = new uint8_t[16 * 0x1000];
};