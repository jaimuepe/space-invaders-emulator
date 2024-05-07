#include "space_invaders.h"

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

    // add custom
}

void SpaceInvaders::run()
{
    for (;;)
    {
        emulator.step();
    }
}

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