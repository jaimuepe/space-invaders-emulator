#include "space_invaders.h"

#include "space_invaders_view.h"
#include "space_invaders_view_sdl.h"

#include <chrono>
#include <iostream>

SpaceInvaders::SpaceInvaders() {}

void SpaceInvaders::init()
{
    emulator.init();

    emulator.set_custom_opcode_handler(
        0xD3, // OUT
        [&](State8080 &state)
        {
            uint8_t port = state.memory[state.pc + 1];
            handle_out_port(state, port, state.a);
            state.pc += 2;
        });

    view = std::make_unique<SpaceInvadersViewSDL>();
    view->init();
}

void SpaceInvaders::run()
{
    // we need our clock to run at 2MHz, so our cycle time is 1 2_000_000 = 500ns
    constexpr int cycle_ns = 500;

    auto last_cpu_time = std::chrono::high_resolution_clock::now();

    long long elapsed_time{0};

    for (;;)
    {
        if (!view->render(emulator.video_memory()))
        {
            break;
        }

        for (int i = 0; i < 100; i++)
        {
            emulator.step();
        }
        /*auto cpu_time = std::chrono::high_resolution_clock::now();

        elapsed_time += std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_time - last_cpu_time).count();

        if (elapsed_time > cycle_ns)
        {
            // tickkk
            std::cout << "elapsed ns: " << elapsed_time << '\n';
            while (elapsed_time > cycle_ns)
            {
                elapsed_time -= cycle_ns;
            }
        }


        last_cpu_time = cpu_time;
        */
    }
}

SpaceInvaders::~SpaceInvaders() {}

void SpaceInvaders::handle_in_port(State8080 &state, uint8_t port, uint8_t value)
{
}

void SpaceInvaders::handle_out_port(State8080 &state, uint8_t port, uint8_t value)
{
    switch (port)
    {
    case 2:
        shift_offset = value & 0x7;
        break;
    case 4:
        shift_x = shift_y;
        shift_y = value;
        break;
    default:
        // the other ports are unused
        break;
    }
}