#include "space_invaders.h"

#include "space_invaders_view.h"
#include "space_invaders_view_sdl.h"

#include <chrono>
#include <iostream>
#include <thread>

SpaceInvaders::SpaceInvaders() {}

void SpaceInvaders::init()
{
    // always one
    input[0] = input[0] | 0b01 | 0b100 | 0b1000;
    input[1] = input[1] | 0b1000;

    emulator.init();

    emulator.connect_out_port(
        2,
        [&](State8080 &state)
        {
            handle_OUT_2(state);
        });

    emulator.connect_out_port(
        4,
        [&](State8080 &state)
        {
            handle_OUT_4(state);
        });

    emulator.connect_in_port(
        1,
        [&](State8080 &state)
        {
            handle_IN_1(state);
        });

    emulator.connect_in_port(
        3,
        [&](State8080 &state)
        {
            handle_IN_3(state);
        });

    view = std::make_unique<SpaceInvadersViewSDL>();
    view->init();
}

void SpaceInvaders::update(uint64_t delta_time_ns)
{
    // how many cycles can we fit in the elapsed time?
    // the cpu runs at 2MHz, 2M cycles per second -> 2 cycles per us
    long long cycles = delta_time_ns / 1000 * 2;

    while (cycles > 0)
    {
        cycles -= emulator.step();
    }

    constexpr long interrupt_time_ns = 8333333;

    interrupt_time_accumulator += delta_time_ns;

    if (interrupt_time_accumulator > interrupt_time_ns)
    {
        interrupt_time_accumulator = 0;

        if (emulator.interruptions_enabled())
        {
            // RST 1 or 2
            uint8_t interrupt = last_interrrupt == 0xD7 ? 0xCF : 0xD7;
            emulator.interrupt(interrupt);

            if (interrupt == 0xD7)
            {
                view->update_screen_buffer(emulator.video_memory());
            }

            last_interrrupt = interrupt;
        }
    }
}

void SpaceInvaders::run()
{
    auto last_time = std::chrono::high_resolution_clock::now();

    while (true)
    {
        view->poll_events(input);

        if (view->should_quit())
        {
            break;
        }

        auto current = std::chrono::high_resolution_clock::now();
        auto delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(current - last_time).count();
        update(delta_time);

        view->render();

        last_time = current;
    }
}

SpaceInvaders::~SpaceInvaders() {}

void SpaceInvaders::handle_IN_1(State8080 &state)
{
    // state.a = input[1];
}

void SpaceInvaders::handle_IN_3(State8080 &state)
{
    uint16_t v = (shift_y << 8) | shift_x;
    state.a = ((v >> (8 - shift_offset)) & 0xFF);
}

void SpaceInvaders::handle_OUT_2(State8080 &state)
{
    shift_offset = state.a & 0x7;
}

void SpaceInvaders::handle_OUT_4(State8080 &state)
{
    shift_x = shift_y;
    shift_y = state.a;
}