#pragma once

#include "state8080.h"

#include <functional>

class Emulator8080
{
public:
    void init();

    int step();

    bool interruptions_enabled() const;

    void interrupt(uint8_t op_code);

    void connect_out_port(uint8_t port, std::function<void(State8080 &state)> handler);

    uint8_t *video_memory();

private:
    State8080 state{};

    std::function<void(State8080 &state)> out_mappings[8];
};