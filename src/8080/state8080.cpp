#include <ostream>
#include "../utils.h"

#include "../reader.h"
#include "state8080.h"

void State8080::init()
{
    memory.init();

#if CPU_DIAG
    pc = 0x100;
#endif
}

std::ostream &operator<<(std::ostream &stream, const State8080 &state)
{
    uint8_t flags = state.flags.cy << 0 |
                    0x02 |
                    state.flags.p << 2 |
                    state.flags.z << 6 |
                    state.flags.s << 7;

    // clang-format off
    stream << 
        "PC: " << Utils::to_hex_string(state.pc) << 
        " AF: " << Utils::to_hex_string(Utils::to_16(flags, state.a)) <<
        " BC: " << Utils::to_hex_string(Utils::to_16(state.c, state.b)) <<
        " DE: " << Utils::to_hex_string(Utils::to_16(state.e, state.d)) <<
        " HL: " << Utils::to_hex_string(Utils::to_16(state.l, state.h)) <<
        " SP: " << Utils::to_hex_string(state.sp) << 
        " CYC: " << state.cycles;
    // clang-format on

    return stream;
}