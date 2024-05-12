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

    uint8_t input[3]{0};

    Emulator8080 emulator{};

    std::unique_ptr<SpaceInvadersView> view;

    void handle_IN_1(State8080 &state);

    void handle_IN_3(State8080 &state);

    void handle_OUT_2(State8080 &state);

    void handle_OUT_4(State8080 &state);
};