#include <vector>
#include <iostream>

#include "utils.h"
#include "8080/state.h"

int main(int argc, char *argv[])
{
    State state{};

    for (;;)
    {
        uint8_t op = state.get_mem(state.pc);

        std::cout << state << '\n';
        std::cout << Utils::to_hex_string(state.pc) << " " << Utils::to_hex_string(op) << " ";

        switch (op)
        {
        case 0x00: // NOP
        {
            std::cout << "NOP";
            state.pc++;
        }
        break;
        case 0x06: // MVI B
        {
            uint8_t b = state.get_mem(state.pc + 1);
            std::cout << "MVI B, " << Utils::to_hex_string(b);

            state.b = b;

            state.pc += 2;
        }
        break;
        case 0x11: // LXI D
        {
            uint16_t de = Utils::to_addressLH(state.get_mem(state.pc + 1), state.get_mem(state.pc + 2));
            std::cout << "LXI DE, " << Utils::to_hex_string(de);

            state.d = (de >> 8) & 0xFF;
            state.e = de & 0xFF;

            state.pc += 3;
        }
        break;
        case 0x1A: // LDAX D
        {
            std::cout << "LDAX D";

            uint16_t de = Utils::to_addressLH(state.e, state.d);
            uint8_t de_mem = state.get_mem(de);

            state.a = de_mem;

            state.pc++;
        }
        break;
        case 0x21: // LXI H
        {
            uint16_t hl = Utils::to_addressLH(state.get_mem(state.pc + 1), state.get_mem(state.pc + 2));
            std::cout << "LXI HL, " << Utils::to_hex_string(hl);

            state.h = (hl >> 8) & 0xFF;
            state.l = hl & 0xFF;

            state.pc += 3;
        }
        break;
        case 0x24: // INR H
        {
            std::cout << "INR H";

            uint16_t h = static_cast<uint16_t>(state.h + 1);

            state.flags.z = (h & 0xFF) == 0;
            state.flags.s = (h & 0x80) != 0;
            state.flags.p = Utils::parity(h & 0xFF);

            state.h = h & 0xFF;
            state.pc++;
        }
        break;
        case 0x31: // LXI SP
        {
            uint16_t sp = Utils::to_addressLH(state.get_mem(state.pc + 1), state.get_mem(state.pc + 2));
            std::cout << "LXI SP, " << Utils::to_hex_string(sp);

            state.sp = sp;
            state.pc += 3;
        }
        break;
        case 0xC3: // JMP
        {
            uint16_t jmpAddr = Utils::to_addressLH(state.get_mem(state.pc + 1), state.get_mem(state.pc + 2));

            std::cout << "JMP " << Utils::to_hex_string(jmpAddr);

            state.pc = jmpAddr;
        }
        break;
        case 0xCD: // CALL
        {
            uint16_t callAddr = Utils::to_addressLH(state.get_mem(state.pc + 1), state.get_mem(state.pc + 2));
            std::cout << "CALL " << Utils::to_hex_string(callAddr);

            uint16_t retAddr = state.pc + 2;

            state.set_mem(state.sp - 1, (retAddr >> 8) & 0xff);
            state.set_mem(state.sp - 2, retAddr & 0xff);

            state.sp = state.sp - 2;

            state.pc = callAddr;
        }
        break;
        default:
        {
            std::cout << "Unhandled op: " << Utils::to_hex_string(op) << '\n';
            exit(1);
        }
        }
        std::cout << '\n';
    }

    return 0;
}