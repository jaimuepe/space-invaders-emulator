#include "8080/state8080.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

void LXI(State8080 &state, uint16_t *pos)
{
    uint8_t hi = state.memory[state.pc + 2];
    uint8_t lo = state.memory[state.pc + 1];

    *pos = hi << 8 | lo;

    state.pc += 3;
}

void MVI(State8080 &state, uint8_t *pos)
{
    uint8_t val = state.memory[state.pc + 1];

    *pos = val;

    state.pc += 2;
}

void INX(State8080 &state, uint16_t *pos)
{
    (*pos)++;
    state.pc++;
}

void DCR(State8080 &state, uint8_t *pos)
{
    uint8_t &r = *pos;
    r--;

    state.flags.z = r == 0;
    state.flags.s = (r & 0x80) != 0;
    state.flags.p = Utils::parity(r);

    state.pc++;
}

std::string PC1_str(const State8080 &state)
{
    std::stringstream ss{};
    ss << Utils::to_hex_string(state.memory[state.pc + 1]) << " ";
    return ss.str();
}

std::string PC2_str(const State8080 &state)
{
    std::stringstream ss{};
    ss << Utils::to_hex_string(state.memory[state.pc + 2]);
    ss << Utils::to_hex_string(state.memory[state.pc + 1]);
    return ss.str();
}

int main(int argc, char *argv[])
{
    State8080 state{};
    state.init();

    uint16_t cycles{0};

    for (;;)
    {
        uint8_t op = state.memory[state.pc];

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
        case 0x05: // DCR B
        {
            std::cout << "DCR B";
            DCR(state, &state.b);
        }
        break;
        case 0x06: // MVI B
        {
            std::cout << "MVI B " << PC1_str(state);
            MVI(state, &state.b);
        }
        break;
        case 0x11: // LXI DE
        {
            std::cout << "LXI DE " << PC2_str(state);
            LXI(state, reinterpret_cast<uint16_t *>(&state.e));
        }
        break;
        case 0x13: // INX DE
        {
            std::cout << "INX DE";
            INX(state, reinterpret_cast<uint16_t *>(&state.e));
        }
        break;
        case 0x1A: // LDAX DE
        {
            std::cout << "LDAX DE";

            uint16_t de = Utils::to_addressLH(state.e, state.d);
            uint8_t de_mem = state.memory[de];

            state.a = de_mem;

            state.pc++;
        }
        break;
        case 0x21: // LXI HL
        {
            std::cout << "LXI HL " << PC2_str(state);
            LXI(state, reinterpret_cast<uint16_t *>(&state.l));
        }
        break;
        case 0x23: // INX HL
        {
            std::cout << "INX HL";
            INX(state, reinterpret_cast<uint16_t *>(&state.l));
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
            std::cout << "LXI SP " << PC2_str(state);
            LXI(state, &state.sp);
        }
        break;
        case 0xC2: // JNZ
        {
            std::cout << "JNZ " << PC2_str(state);

            if (!state.flags.z)
            {
                uint16_t jmpAddr = Utils::to_addressLH(
                    state.memory[state.pc + 1],
                    state.memory[state.pc + 2]);

                state.pc = jmpAddr;
            }
            else
            {
                state.pc += 3;
            }
        }
        break;
        case 0xC3: // JMP
        {
            std::cout << "JMP " << PC2_str(state);

            uint16_t jmpAddr = Utils::to_addressLH(
                state.memory[state.pc + 1],
                state.memory[state.pc + 2]);

            state.pc = jmpAddr;
        }
        break;
        case 0x77: // MOV (HL),A
        {
            std::cout << "MOV (HL), A";

            uint16_t addr = Utils::to_addressLH(
                state.memory[state.pc + 1],
                state.memory[state.pc + 2]);

            state.memory[addr] = state.a;

            state.pc++;
        }
        break;
        case 0xC9: // RET
        {
            std::cout << "RET";

            uint16_t retAddr = Utils::to_addressLH(
                state.memory[state.sp],
                state.memory[state.sp + 1]);

            state.pc = retAddr;
            state.sp += 2;
        }
        break;
        case 0xCD: // CALL
        {
            std::cout << "CALL " << PC2_str(state);

            uint16_t callAddr = Utils::to_addressLH(
                state.memory[state.pc + 1],
                state.memory[state.pc + 2]);

            uint16_t retAddr = state.pc + 3;

            state.memory[state.sp - 1] = (retAddr >> 8) & 0xff;
            state.memory[state.sp - 2] = retAddr & 0xff;

            state.sp -= 2;

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