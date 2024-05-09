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

    emulator.connect_in_port(
        3,
        [&](State8080 &state)
        {
            handle_IN_3(state);
        });

    view = std::make_unique<SpaceInvadersViewSDL>();
    view->init();
}

void SpaceInvaders::run()
{
    auto last_cpu_time = std::chrono::high_resolution_clock::now();

    uint64_t render_time_accumulator{0};

    for (;;)
    {
        auto time = std::chrono::high_resolution_clock::now();

        view->poll_events();

        if (view->should_quit())
        {
            break;
        }

        auto render_time_delta = std::chrono::duration_cast<std::chrono::nanoseconds>(time - last_cpu_time);
        render_time_accumulator += render_time_delta.count();

        constexpr long frame_time_ns = 16666666; // 60 fps

        if (render_time_accumulator > frame_time_ns)
        {
            while (render_time_accumulator > frame_time_ns)
            {
                render_time_accumulator -= frame_time_ns;
            }

            view->render(emulator.video_memory());

            if (emulator.interruptions_enabled())
            {
                emulator.interrupt(0xD7); // RST 2
            }
        }

        auto elapsed_time_us = std::chrono::duration_cast<std::chrono::microseconds>(time - last_cpu_time).count();

        // how many cycles can we fit in the elapsed time?
        // the cpu runs at 2MHz, 2M cycles per second -> 2 cycles per us
        int cycles = elapsed_time_us * 2;

        while (cycles > 0)
        {
            cycles -= emulator.step();
        }

        last_cpu_time = time;
    }
}

SpaceInvaders::~SpaceInvaders() {}

void SpaceInvaders::handle_IN_3(State8080 &state)
{
    uint16_t v = (shift_y << 8) | shift_x;
    state.a = ((v >> (8 - shift_offset)) & 0xff);
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