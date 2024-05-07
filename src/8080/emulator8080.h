#pragma once

#include "state8080.h"

#include <functional>

class Emulator8080
{
public:
    void init();

    void step();

    void set_custom_opcode_handler(uint8_t op, std::function<void(State8080 &state)> handler);

private:
    State8080 state{};

    std::function<void(State8080 &state)> custom_op_handlers[0xFF];
};