#include "emulator8080.h"

#include <iostream>
#include <sstream>

#include "../utils.h"

#define ENABLE_LOGGING 0

uint8_t *Emulator8080::video_memory()
{
    return &state.memory[0x2400];
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

void set_alu_flags(State8080 &state, uint16_t result)
{
    state.flags.z = result == 0;
    state.flags.s = (result & 0x80) != 0;
    state.flags.p = Utils::parity(result);
    state.flags.c = (result & 0xFFFF0000) != 0;
}

void NOP(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "NOP" << '\n';
#endif
    state.pc++;
    state.cycles += 4;
}

void LXI(State8080 &state, uint16_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "LXI " << name << ", " << PC2_str(state) << '\n';
#endif

    uint8_t hi = state.memory[state.pc + 2];
    uint8_t lo = state.memory[state.pc + 1];

    *pos = hi << 8 | lo;

    state.pc += 3;
    state.cycles += 10;
}

void DAD(State8080 &state, uint16_t add, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "DAD " << name << '\n';
#endif

    uint32_t hl = Utils::to_16(
        state.l,
        state.h);

    hl += add;

    state.h = (hl & 0XFF00) >> 8;
    state.l = hl & 0xFF;

    state.flags.c = (hl & 0xFFFF0000) != 0;

    state.pc++;
    state.cycles += 10;
}

void XCHG(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "XCHG " << '\n';
#endif

    uint8_t h = state.h;
    uint8_t l = state.l;

    state.h = state.d;
    state.l = state.e;
    state.d = h;
    state.e = l;

    state.pc++;
    state.cycles += 5;
}

void LDAX(State8080 &state, uint16_t pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "LDAX " << name << '\n';
#endif
    state.a = state.memory[pos];

    state.pc++;
    state.cycles += 7;
}

void LDA(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "LDA (" << PC2_str(state) << ")" << '\n';
#endif

    uint16_t addr = Utils::to_16(
        state.memory[state.pc + 1],
        state.memory[state.pc + 2]);

    state.a = state.memory[addr];

    state.pc += 3;
    state.cycles += 13;
}

void MVI_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MVI " << name << ", " << PC1_str(state) << '\n';
#endif
    uint8_t val = state.memory[state.pc + 1];

    *pos = val;

    state.pc += 2;
    state.cycles += 7;
}

void MVI_M(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "MVI (HL) " << PC1_str(state) << '\n';
#endif
    uint16_t hl = Utils::to_16(
        state.l,
        state.h);

    state.memory[hl] = state.memory[state.pc + 1];

    state.pc += 2;
    state.cycles += 10;
}

void INR_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "INR " << name << '\n';
#endif
    uint16_t h = (*pos) + 1;

    state.flags.z = (h & 0xFF) == 0;
    state.flags.s = (h & 0x80) != 0;
    state.flags.p = Utils::parity(h & 0xFF);

    *pos = h & 0xFF;

    state.pc++;
    state.cycles += 5;
}

void INX(State8080 &state, uint16_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "INX " << name << '\n';
#endif
    (*pos)++;

    state.pc++;
    state.cycles += 5;
}

void DCR_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "DCR " << name << '\n';
#endif
    uint8_t &r = *pos;
    r--;

    state.flags.z = r == 0;
    state.flags.s = (r & 0x80) != 0;
    state.flags.p = Utils::parity(r);

    state.pc++;
    state.cycles += 5;
}

void MOV_R_R(State8080 &state, uint8_t *pos, uint8_t value, const char *nameL, const char *nameR)
{
#if ENABLE_LOGGING
    std::cout << "MOV " << nameL << ", " << nameR << '\n';
#endif
    *pos = value;

    state.pc++;
    state.cycles += 5;
}

void MOV_M_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MOV (HL), " << name << '\n';
#endif
    uint16_t hl = Utils::to_16(state.l, state.h);
    state.memory[hl] = value;

    state.pc++;
    state.cycles += 7;
}

void MOV_R_M(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MOV " << name << ", (HL)";
#endif
    uint16_t hl = Utils::to_16(state.l, state.h);
    *pos = state.memory[hl];

    state.pc++;
    state.cycles += 7;
}

void STA(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "STA (" << PC2_str(state) << ")" << '\n';
#endif

    uint16_t addr = Utils::to_16(
        state.memory[state.pc + 1],
        state.memory[state.pc + 2]);

    state.memory[addr] = state.a;

    state.pc += 3;
    state.cycles += 13;
}

void CPI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CPI A, " << PC1_str(state) << '\n';
#endif
    uint8_t a = state.a;
    uint8_t arg = state.memory[state.pc + 1];

    uint16_t res = static_cast<uint16_t>(a) - arg;

    set_alu_flags(state, res);

    state.pc += 2;
    state.cycles += 7;
}

void XRA_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "XRA " << name << '\n';
#endif

    uint8_t a = state.a ^ value;

    state.a = a;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc += 1;
    state.cycles += 4;
}

void RRC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RRC" << '\n';
#endif

    uint8_t bit_0 = state.a & 0x01;
    state.a = state.a >> 1 | bit_0 << 7;

    state.flags.c = bit_0;

    state.pc++;
    state.cycles += 4;
}

void ADI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ADI " << PC1_str(state) << '\n';
#endif

    uint16_t a = static_cast<uint16_t>(state.a) + state.memory[state.pc + 1];
    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc += 2;
    state.cycles += 7;
}

void ANA_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "ANA " << name << '\n';
#endif

    uint8_t a = state.a & value;
    state.a = a;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc++;
    state.cycles += 4;
}

void ANI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ANI " << PC1_str(state) << '\n';
#endif

    // no need to cast, we are never going to have carry here
    uint16_t a = state.a & state.memory[state.pc + 1];
    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc += 2;
    state.cycles += 7;
}

void CALL(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CALL " << PC2_str(state) << '\n';
#endif
    uint16_t callAddr = Utils::to_16(
        state.memory[state.pc + 1],
        state.memory[state.pc + 2]);

    uint16_t retAddr = state.pc + 3;

    state.memory[state.sp - 1] = (retAddr >> 8) & 0xff;
    state.memory[state.sp - 2] = retAddr & 0xff;

    state.sp -= 2;

    state.pc = callAddr;
    state.cycles += 17;
}

void RET(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RET" << '\n';
#endif

    uint16_t retAddr = Utils::to_16(
        state.memory[state.sp],
        state.memory[state.sp + 1]);

    state.pc = retAddr;

    state.sp += 2;
    state.cycles += 10;
}

void PUSH(State8080 &state, uint8_t lo, uint8_t hi, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "PUSH " << name << '\n';
#endif

    state.memory[state.sp - 1] = hi;
    state.memory[state.sp - 2] = lo;

    state.sp -= 2;

    state.pc++;
    state.cycles += 11;
}

void PUSH_PSW(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "PUSH PSW" << '\n';
#endif

    uint8_t flags =
        (state.flags.c) |
        0x02 | // second bit is always one
        state.flags.p << 2 |
        state.flags.z << 6 |
        state.flags.s << 7;

    state.memory[state.sp - 2] = flags;
    state.memory[state.sp - 1] = state.a;

    state.sp -= 2;

    state.pc++;
    state.cycles += 11;
}

void POP(State8080 &state, uint8_t *lo, uint8_t *hi, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "POP " << name << '\n';
#endif

    *lo = state.memory[state.sp];
    *hi = state.memory[state.sp + 1];

    state.sp += 2;

    state.pc++;
    state.cycles += 10;
}

void POP_PSW(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "POP PSW" << '\n';
#endif

    uint8_t flags = state.memory[state.sp];
    uint8_t a = state.memory[state.sp + 1];

    state.flags.c = flags & 0x01;
    state.flags.p = (flags >> 2) & 0x01;
    state.flags.z = (flags >> 6) & 0x01;
    state.flags.s = (flags >> 7) & 0x01;

    state.a = a;

    state.sp += 2;

    state.pc++;
    state.cycles += 10;
}

void JMP(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JMP " << PC2_str(state) << '\n';
#endif
    uint16_t jmpAddr = Utils::to_16(
        state.memory[state.pc + 1],
        state.memory[state.pc + 2]);

    state.pc = jmpAddr;
    state.cycles += 10;
}

void JNZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JNZ " << PC2_str(state) << '\n';
#endif
    if (!state.flags.z)
    {
        uint16_t jmpAddr = Utils::to_16(
            state.memory[state.pc + 1],
            state.memory[state.pc + 2]);

        state.pc = jmpAddr;
    }
    else
    {
        state.pc += 3;
    }
    state.cycles += 10;
}

void Emulator8080::init()
{
    state.init();
}

void Emulator8080::step()
{
    uint8_t op = state.memory[state.pc];

#if ENABLE_LOGGING
    std::cout << state.counter << " " << Utils::to_hex_string(state.pc) << " " << Utils::to_hex_string(op) << " ";
#endif

    switch (op)
    {
    case 0x00: // NOP
        NOP(state);
        break;
    case 0x01: // LXI BC
        LXI(state, reinterpret_cast<uint16_t *>(&state.c), "BC");
        break;
    case 0x05: // DCR B
        DCR_R(state, &state.b, "B");
        break;
    case 0x06: // MVI B
        MVI_R(state, &state.b, "B");
        break;
    case 0x09: // DAD B
        DAD(state, Utils::to_16(state.c, state.b), "BC");
        break;
    case 0x0D: // DCR C
        DCR_R(state, &state.c, "C");
        break;
    case 0x0E: // MVI C
        MVI_R(state, &state.c, "C");
        break;
    case 0x0F: // RRC
        RRC(state);
        break;
    case 0x11: // LXI DE
        LXI(state, reinterpret_cast<uint16_t *>(&state.e), "DE");
        break;
    case 0x13: // INX DE
        INX(state, reinterpret_cast<uint16_t *>(&state.e), "DE");
        break;
    case 0x19: // DAD DE
        DAD(state, Utils::to_16(state.e, state.d), "DE");
        break;
    case 0x1A: // LDAX DE
        LDAX(state, Utils::to_16(state.e, state.d), "DE");
        break;
    case 0x21: // LXI HL
        LXI(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0x23: // INX HL
        INX(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0x24: // INR H
        INR_R(state, &state.h, "H");
        break;
    case 0x26: // MVI H
        MVI_R(state, &state.h, "H");
        break;
    case 0x29: // DAD H
        DAD(state, Utils::to_16(state.l, state.h), "HL");
        break;
    case 0x31: // LXI SP
        LXI(state, &state.sp, "SP");
        break;
    case 0x32: // STA
        STA(state);
        break;
    case 0x36: // MVI (HL)
        MVI_M(state);
        break;
    case 0x3A: // LDA adr
        LDA(state);
        break;
    case 0x3E: // MVI A
        MVI_R(state, &state.a, "A");
        break;
    case 0x56: // MOV D, (HL)
        MOV_R_M(state, &state.d, "D");
        break;
    case 0x5E: // MOV E, (HL)
        MOV_R_M(state, &state.e, "E");
        break;
    case 0x66: // MOV H, (HL)
        MOV_R_M(state, &state.h, "H");
        break;
    case 0x6F: // MOV L, A
        MOV_R_R(state, &state.l, state.a, "L", "A");
        break;
    case 0x70: // MOV (HL), B
        MOV_M_R(state, state.b, "B");
        break;
    case 0x71: // MOV (HL), C
        MOV_M_R(state, state.c, "C");
        break;
    case 0x72: // MOV (HL), D
        MOV_M_R(state, state.d, "D");
        break;
    case 0x73: // MOV (HL), E
        MOV_M_R(state, state.e, "E");
        break;
    case 0x74: // MOV (HL), H
        MOV_M_R(state, state.h, "H");
        break;
    case 0x75: // MOV (HL), L
        MOV_M_R(state, state.l, "L");
        break;
    case 0x77: // MOV (HL), A
        MOV_M_R(state, state.a, "A");
        break;
    case 0x7A: // MOV A, D
        MOV_R_R(state, &state.a, state.d, "A", "D");
        break;
    case 0x7B: // MOV A, E
        MOV_R_R(state, &state.a, state.e, "A", "E");
        break;
    case 0x7C: // MOV A, H
        MOV_R_R(state, &state.a, state.h, "A", "H");
        break;
    case 0x7E: // MOV A, (HL)
        MOV_R_M(state, &state.a, "A");
        break;
    case 0xAF: // XRA A
        XRA_R(state, state.a, "A");
        break;
    case 0xA7: // ANA A
        ANA_R(state, state.a, "A");
        break;
    case 0xC1: // POP BC
        POP(state, &state.c, &state.b, "BC");
        break;
    case 0xC2: // JNZ
        JNZ(state);
        break;
    case 0xC3: // JMP
        JMP(state);
        break;
    case 0xC5: // PUSH BC
        PUSH(state, state.c, state.b, "BC");
        break;
    case 0xC6: // ADI
        ADI(state);
        break;
    case 0xC9: // RET
        RET(state);
        break;
    case 0xCD: // CALL
        CALL(state);
        break;
    case 0xD1: // POP DE
        POP(state, &state.e, &state.d, "DE");
        break;
    case 0xD5: // PUSH DE
        PUSH(state, state.e, state.d, "DE");
        break;
    case 0xEB: // XCHG
        XCHG(state);
        break;
    case 0xE1: // POP
        POP(state, &state.l, &state.h, "(HL)");
        break;
    case 0xE5: // PUSH HL
        PUSH(state, state.l, state.h, "HL");
        break;
    case 0xE6: // ANI
        ANI(state);
        break;
    case 0xF1: // POP_PSW
        POP_PSW(state);
        break;
    case 0xF5:
        PUSH_PSW(state);
        break;
    case 0xFB: // EI
        // TODO this is game specific, skip it for now
        state.pc += 1;
        break;
    case 0xFE: // CPI
        CPI(state);
        break;
    default:

        auto handler = custom_op_handlers[op];
        if (handler)
        {
            handler(state);
        }
        else
        {
#if !ENABLE_LOGGING
            std::cerr << state.counter << " " << Utils::to_hex_string(state.pc) << " " << Utils::to_hex_string(op) << " ";
#endif
            std::cerr << "UNHANDLED OP" << '\n';

#if !ENABLE_LOGGING
            std::cout << state;
            std::cout << "\n\n";
#endif
            exit(1);
        }
    }

#if ENABLE_LOGGING
    std::cout << state;
    std::cout << "\n\n";
#endif

    state.counter++;
}

void Emulator8080::set_custom_opcode_handler(
    uint8_t op,
    std::function<void(State8080 &state)> handler)
{
    custom_op_handlers[op] = handler;
}