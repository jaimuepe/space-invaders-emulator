#pragma once

#include "../8080/emulator8080.h"

#include <memory>

class SpaceInvadersView;

class SpaceInvaders
{
public:
    SpaceInvaders();

    void init();

    void run();

    ~SpaceInvaders();

private:
    uint16_t shift_x{0};
    uint16_t shift_y{0};

    uint8_t shift_offset{0};

    uint8_t inputs[2];

    Emulator8080 emulator{};

    std::unique_ptr<SpaceInvadersView> view;

    void handle_in_port(State8080 &state, uint8_t port, uint8_t value);

    void handle_out_port(State8080 &state, uint8_t port, uint8_t value);
};