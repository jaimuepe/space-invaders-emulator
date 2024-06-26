#pragma once

#include "mem8080.h"

#include <cstdint>
#include <ostream>
#include <vector>

struct State8080
{
public:
    uint8_t a{0};

    uint8_t c{0};
    uint8_t b{0};

    uint8_t e{0};
    uint8_t d{0};

    uint8_t l{0};
    uint8_t h{0};

    uint16_t sp{0};
    uint16_t pc{0};

    uint64_t counter{0};
    uint64_t cycles{0};

    Mem8080 memory{};

    bool interruptions_enabled{false};

    std::vector<uint8_t> bus{};

    struct Flags
    {
        bool s;
        bool z;
        bool p;
        bool cy;
    } flags;

    void init();

    friend std::ostream &operator<<(std::ostream &stream, const State8080 &state);
};