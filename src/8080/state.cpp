#include <ostream>
#include "../utils.h"

#include "../reader.h"
#include "state.h"

State::State() 
{
    copy_at("invaders.h", memory + 0x0000);
    copy_at("invaders.g", memory + 0x0800);
    copy_at("invaders.f", memory + 0x1000);
    copy_at("invaders.e", memory + 0x1800);
}

std::ostream &operator<<(std::ostream &stream, const State &state)
{
    stream << 
        "A=" << Utils::to_hex_string(state.a) << 
        " B=" << Utils::to_hex_string(state.b) << 
        " C=" << Utils::to_hex_string(state.c) << 
        " D=" << Utils::to_hex_string(state.d) << 
        " E=" << Utils::to_hex_string(state.e) << 
        " H=" << Utils::to_hex_string(state.h) << 
        " L=" << Utils::to_hex_string(state.l) << 
        " SP= " << Utils::to_hex_string(state.sp) <<
        " S" << static_cast<int>(state.flags.s) << 
        " Z" << static_cast<int>(state.flags.z) << 
        " P" << static_cast<int>(state.flags.p) <<
        " C" << static_cast<int>(state.flags.c);
    
    return stream; 
}

void State::set_mem(uint16_t addr, uint8_t value)
{
    memory[addr] = value;
    
}

uint8_t State::get_mem(uint16_t addr) const 
{
    return memory[addr];
}