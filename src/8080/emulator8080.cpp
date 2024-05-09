#include "emulator8080.h"

#include <iostream>
#include <sstream>

#include "../utils.h"

#define ENABLE_LOGGING 0

// clang-format off
uint8_t cycles8080[] = 
{
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
    4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
    4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
    4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
    7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
    11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, 
    11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11, 
    11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11, 
};
// clang-format on

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

void print_state(const State8080 &state)
{
    std::cout << state.counter << " " << Utils::to_hex_string(state.pc) << '\n';
    std::cout << state << '\n';
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
}

void LDAX(State8080 &state, uint16_t pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "LDAX " << name << '\n';
#endif
    state.a = state.memory[pos];

    state.pc++;
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
}

void MVI_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MVI " << name << ", " << PC1_str(state) << '\n';
#endif
    uint8_t val = state.memory[state.pc + 1];

    *pos = val;

    state.pc += 2;
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
}

void STC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "STC" << '\n';
#endif

    state.flags.c = true;
    state.pc++;
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
}

void INX(State8080 &state, uint16_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "INX " << name << '\n';
#endif
    (*pos)++;

    state.pc++;
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
}

void DCR_M(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "DCR (HL)" << '\n';
#endif

    uint16_t hl = Utils::to_16(state.l, state.h);

    uint8_t &r = state.memory[hl];
    r--;

    state.flags.z = r == 0;
    state.flags.s = (r & 0x80) != 0;
    state.flags.p = Utils::parity(r);

    state.pc++;
}

void MOV_R_R(State8080 &state, uint8_t *pos, uint8_t value, const char *nameL, const char *nameR)
{
#if ENABLE_LOGGING
    std::cout << "MOV " << nameL << ", " << nameR << '\n';
#endif
    *pos = value;

    state.pc++;
}

void MOV_M_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MOV (HL), " << name << '\n';
#endif
    uint16_t hl = Utils::to_16(state.l, state.h);
    state.memory[hl] = value;

    state.pc++;
}

void MOV_R_M(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MOV " << name << ", (HL)";
#endif
    uint16_t hl = Utils::to_16(state.l, state.h);
    *pos = state.memory[hl];

    state.pc++;
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

    state.pc++;
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
}

void RST(State8080 &state, int rst_num)
{
#if ENABLE_LOGGING
    std::cout << "RST " << rst_num << '\n';
#endif

    uint16_t callAddr = rst_num * 8;

    uint16_t retAddr = state.pc;

    state.memory[state.sp - 1] = (retAddr >> 8) & 0xff;
    state.memory[state.sp - 2] = retAddr & 0xff;

    state.sp -= 2;

    state.pc = callAddr;
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
}

void RC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RC" << '\n';
#endif

    if (state.flags.c)
    {
        RET(state);
    }
    else
    {
        state.pc++;
    }
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
}

void RZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RZ" << '\n';
#endif

    if (state.flags.z)
    {
        RET(state);
    }
    else
    {
        state.pc++;
    }
}

void RNZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RNZ" << '\n';
#endif

    if (!state.flags.z)
    {
        RET(state);
    }
    else
    {
        state.pc++;
    }
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
}

void JNZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JNZ " << PC2_str(state) << '\n';
#endif
    if (!state.flags.z)
    {
        JMP(state);
    }
    else
    {
        state.pc += 3;
    }
}

void JZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JZ " << PC2_str(state) << '\n';
#endif
    if (state.flags.z)
    {
        JMP(state);
    }
    else
    {
        state.pc += 3;
    }
}

void JC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JC " << PC2_str(state) << '\n';
#endif

    if (state.flags.c)
    {
        JMP(state);
    }
    else
    {
        state.pc += 3;
    }
}

void IN(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "IN " << PC1_str(state) << '\n';
#endif

    state.pc += 2;
}

void OUT(State8080 &state, std::function<void(State8080 &)> mapping)
{
#if ENABLE_LOGGING
    std::cout << "OUT " << PC1_str(state) << '\n';
#endif

    uint8_t port = state.memory[state.pc + 1];

    if (mapping)
    {
        mapping(state);
    }

    state.pc += 2;
}

void Emulator8080::init()
{
    state.init();
}

bool Emulator8080::interruptions_enabled() const
{
    return state.interruptions_enabled;
}

void Emulator8080::interrupt(uint8_t op_code)
{
    state.bus.push_back(op_code);
}

int Emulator8080::step()
{
    bool is_interruption = state.interruptions_enabled && state.bus.size() > 0;

    uint8_t op;
    if (is_interruption)
    {
        op = state.bus[0];
        state.bus.erase(state.bus.begin());
    }
    else
    {
        op = state.memory[state.pc];
    }

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
    case 0x03: // INX BC
        INX(state, reinterpret_cast<uint16_t *>(&state.c), "BC");
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
    case 0x0A: // LDAX BC
        LDAX(state, Utils::to_16(state.c, state.b), "BC");
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
    case 0x2E: // MVI L
        MVI_R(state, &state.l, "L");
        break;
    case 0x31: // LXI SP
        LXI(state, &state.sp, "SP");
        break;
    case 0x32: // STA
        STA(state);
        break;
    case 0x35: // DCR M
        DCR_M(state);
        break;
    case 0x36: // MVI (HL)
        MVI_M(state);
        break;
    case 0x37: // STC
        STC(state);
        break;
    case 0x3A: // LDA adr
        LDA(state);
        break;
    case 0x3D: // DCR A
        DCR_R(state, &state.a, "A");
        break;
    case 0x3E: // MVI A
        MVI_R(state, &state.a, "A");
        break;
    case 0x4F: // MOV C, A
        MOV_R_R(state, &state.c, state.a, "C", "A");
        break;
    case 0x56: // MOV D, (HL)
        MOV_R_M(state, &state.d, "D");
        break;
    case 0x57: // MOV D, A
        MOV_R_R(state, &state.d, state.a, "D", "A");
        break;
    case 0x5E: // MOV E, (HL)
        MOV_R_M(state, &state.e, "E");
        break;
    case 0x5F: // MOV E, A
        MOV_R_R(state, &state.e, state.a, "E", "A");
        break;
    case 0x66: // MOV H, (HL)
        MOV_R_M(state, &state.h, "H");
        break;
    case 0x67: // MOV H, A
        MOV_R_R(state, &state.h, state.a, "H", "A");
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
    case 0x79: // MOV A, C
        MOV_R_R(state, &state.a, state.c, "A", "C");
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
    case 0x7D: // MOV A, L
        MOV_R_R(state, &state.a, state.l, "A", "L");
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
    case 0xC0: // RNZ
        RNZ(state);
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
    case 0xC8: // RZ
        RZ(state);
        break;
    case 0xC9: // RET
        RET(state);
        break;
    case 0xCA: // JZ
        JZ(state);
        break;
    case 0xCD: // CALL
        CALL(state);
        break;
    case 0xD1: // POP DE
        POP(state, &state.e, &state.d, "DE");
        break;
    case 0xD3: // OUT
        OUT(state, out_mappings[state.memory[state.pc + 1]]);
        break;
    case 0xD5: // PUSH DE
        PUSH(state, state.e, state.d, "DE");
        break;
    case 0xD7: // RST 2
        RST(state, 2);
        break;
    case 0xD8: // RC
        RC(state);
        break;
    case 0xDA: // JC
        JC(state);
        break;
    case 0xDB: // IN
        IN(state);
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
        state.interruptions_enabled = true;
        state.pc += 1;
        break;
    case 0xFE: // CPI
        CPI(state);
        break;
    default:
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

    int cycles = cycles8080[op];

    state.cycles += cycles;
    state.counter++;

    return cycles;
}

void Emulator8080::connect_out_port(uint8_t port, std::function<void(State8080 &state)> handler)
{
    out_mappings[port] = handler;
}