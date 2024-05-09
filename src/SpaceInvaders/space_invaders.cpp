#include "space_invaders.h"

#include "space_invaders_view.h"
#include "space_invaders_view_sdl.h"

#include <chrono>
#include <iostream>
#include <thread>

SpaceInvaders::SpaceInvaders() {}

void SpaceInvaders::init()
{
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

    view = std::make_unique<SpaceInvadersViewSDL>();
    view->init();
}

void SpaceInvaders::run()
{
    auto last_cpu_time = std::chrono::high_resolution_clock::now();

    for (;;)
    {
        view->poll_events();

        if (view->should_quit())
        {
            break;
        }

        if (!view->render(emulator.video_memory()))
        {
            continue;
        }

        auto cpu_time = std::chrono::high_resolution_clock::now();

        auto elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(cpu_time - last_cpu_time).count();

        // how many cycles can we fit in the elapsed time?
        // the cpu runs at 2MHz, 2M cycles per second -> 2 cycles per us
        int cycles = elapsed_time_us * 2;

        std::cout << "work time: " << elapsed_time_us << "us -> " << cycles << " cycles" << '\n';

        while (cycles > 0)
        {
            cycles -= emulator.step();
        }

        last_cpu_time = cpu_time;
    }
}

SpaceInvaders::~SpaceInvaders() {}

void SpaceInvaders::handle_OUT_2(State8080 &state)
{
    shift_offset = state.a & 0x7;
}

void SpaceInvaders::handle_OUT_4(State8080 &state)
{
    shift_x = shift_y;
    shift_y = state.a;
}