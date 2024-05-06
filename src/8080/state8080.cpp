#include <ostream>
#include "../utils.h"

#include "../reader.h"
#include "state8080.h"

std::ostream &operator<<(std::ostream &stream, const State8080 &state)
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