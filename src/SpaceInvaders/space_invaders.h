#pragma once

#include "../8080/emulator8080.h"

#include <memory>

constexpr int FPS = 60;
constexpr int CLOCK_SPEED = 2000000;
constexpr int CYCLES_PER_FRAME = CLOCK_SPEED / FPS;

class SpaceInvadersView;

class SpaceInvaders
{
public:
    SpaceInvaders();

    void init();

    void run();

    ~SpaceInvaders();

private:
    uint8_t last_interrrupt{0xD7};

    uint64_t interrupt_time_accumulator{0};

    uint16_t shift_x{0};
    uint16_t shift_y{0};

    uint8_t shift_offset{0};

    uint8_t input[3]{0};

    Emulator8080 emulator{};

    void update(uint64_t delta_time_ms);

    std::unique_ptr<SpaceInvadersView> view;

    void handle_IN_1(State8080 &state);

    void handle_IN_3(State8080 &state);

    void handle_OUT_2(State8080 &state);

    void handle_OUT_4(State8080 &state);
};