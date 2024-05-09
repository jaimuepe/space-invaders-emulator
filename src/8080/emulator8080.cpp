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

const uint8_t *Emulator8080::video_memory()
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

void STAX(State8080 &state, uint16_t pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "STAX " << name << '\n';
#endif

    state.memory[pos] = state.a;
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

void INR_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "INR " << name << '\n';
#endif
    uint16_t r = (*pos) + 1;

    state.flags.z = (r & 0xFF) == 0;
    state.flags.s = (r & 0x80) != 0;
    state.flags.p = Utils::parity(r & 0xFF);

    *pos = r & 0xFF;

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

void MVI_R(State8080 &state, uint8_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "MVI " << name << ", " << PC1_str(state) << '\n';
#endif
    uint8_t val = state.memory[state.pc + 1];

    *pos = val;

    state.pc += 2;
}

void RLC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RLC" << '\n';
#endif

    uint8_t bit_7 = state.a >> 7;
    state.a = state.a << 1 | bit_7;

    state.flags.c = bit_7;

    state.pc++;
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

void LDAX(State8080 &state, uint16_t pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "LDAX (" << name << ")" << '\n';
#endif
    state.a = state.memory[pos];
    state.pc++;
}

void RRC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RRC" << '\n';
#endif

    uint8_t bit_0 = state.a & 0x01;
    state.a = state.a >> 1 | (bit_0 << 7);

    state.flags.c = bit_0;

    state.pc++;
}

void RAR(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RAR" << '\n';
#endif

    uint8_t bit_0 = state.a & 0x01;
    uint8_t bit_7 = state.a >> 7;

    state.a = state.a >> 1 | (bit_7 << 7);

    state.flags.c = bit_0;

    state.pc++;
}

void SHLD(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "SHLD " << PC2_str(state) << '\n';
#endif

    uint8_t l = state.l;
    state.l = state.memory[state.pc + 1];
    state.memory[state.pc + 1] = l;

    uint8_t h = state.h;
    state.h = state.memory[state.pc + 2];
    state.memory[state.pc + 2] = h;

    state.pc += 3;
}

void LHLD(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "LHLD " << PC2_str() << '\n';
#endif

    state.l = state.memory[state.pc + 1];
    state.h = state.memory[state.pc + 2];

    state.pc += 3;
}

void CMA(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CMA" << '\n';
#endif

    state.a = !state.a;
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

void DCX(State8080 &state, uint16_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "DCX " << name << '\n';
#endif

    (*pos)--;
    state.pc++;
}

void PCHL(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "PCHL" << '\n';
#endif

    state.pc = Utils::to_16(state.l, state.h);
}

void XTHL(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "XTHL" << '\n';
#endif

    uint8_t h = state.h;
    state.h = state.memory[state.sp + 1];
    state.memory[state.sp + 1] = h;

    uint8_t l = state.l;
    state.l = state.memory[state.sp];
    state.memory[state.sp] = l;

    state.pc++;
}

void XCHG(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "XCHG " << '\n';
#endif

    uint8_t h = state.h;
    state.h = state.d;
    state.d = h;

    uint8_t l = state.l;
    state.l = state.e;
    state.e = l;

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

void ADD_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "ADD " << name << '\n';
#endif

    uint16_t a = static_cast<uint16_t>(state.a) + value;
    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc++;
}

void ADD_M(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ADD (HL)" << '\n';
#endif

    uint8_t value = state.memory[Utils::to_16(state.l, state.h)];
    uint16_t a = static_cast<uint16_t>(state.a) + value;

    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc++;
}

void ADC_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "ADC " << name << '\n';
#endif

    uint16_t a = static_cast<uint16_t>(state.a) + value + state.flags.c;
    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc++;
}

void SUB_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "SUB " << name << '\n';
#endif

    uint16_t a = static_cast<uint16_t>(state.a) - value;
    set_alu_flags(state, a);

    state.a = a & 0xFF;

    state.pc++;
}

void ORI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ORI " << PC1_str(state) << '\n';
#endif

    uint8_t value = state.memory[state.pc + 1];

    uint8_t a = state.a | value;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc += 2;
}

void ORA_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "ORA " << name << '\n';
#endif

    uint8_t a = state.a | value;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc++;
}

void ORA_M(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ORA (HL)" << '\n';
#endif

    uint8_t value = state.memory[Utils::to_16(state.l, state.h)];

    uint8_t a = state.a | value;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc++;
}

void CMP_R(State8080 &state, uint8_t value, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "CMP " << name << '\n';
#endif

    uint16_t res = static_cast<uint16_t>(state.a) - value;
    set_alu_flags(state, res);

    state.pc++;
}

void CPI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CPI A, " << PC1_str(state) << '\n';
#endif
    uint8_t data = state.memory[state.pc + 1];

    uint16_t res = static_cast<uint16_t>(state.a) - data;

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

void ANA_M(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "ANA (HL)" << '\n';
#endif

    uint16_t hl = Utils::to_16(state.l, state.h);

    uint16_t a = state.a & state.memory[hl];
    state.a = a;

    state.flags.z = a == 0;
    state.flags.s = (a & 0x80) != 0;
    state.flags.p = Utils::parity(a);
    state.flags.c = false;

    state.pc++;
}

void SUI(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "SUI " << PC1_str(state) << '\n';
#endif

    uint16_t a = static_cast<uint16_t>(state.a) - state.memory[state.pc + 1];
    set_alu_flags(state, a);

    state.a = a & 0xFF;
    state.pc += 2;
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

void RST(State8080 &state, int rst_num, bool is_interruption)
{
#if ENABLE_LOGGING
    std::cout << "RST " << rst_num << '\n';
#endif

    uint16_t callAddr = rst_num * 8;

    // if this was an interruption, the op came from the bus
    // and thus we don't have to advance the pc
    uint16_t retAddr = is_interruption ? state.pc : (state.pc + 1);

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

void RNC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RNC" << '\n';
#endif

    if (!state.flags.c)
    {
        RET(state);
    }
    else
    {
        state.pc++;
    }
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

void POP(State8080 &state, uint16_t *pos, const char *name)
{
#if ENABLE_LOGGING
    std::cout << "POP " << name << '\n';
#endif

    *pos = Utils::to_16(state.memory[state.sp], state.memory[state.sp + 1]);

    state.sp += 2;

    state.pc++;
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

void CNC(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CNC " << PC2_str(state) << '\n';
#endif
    if (!state.flags.c)
    {
        CALL(state);
    }
    else
    {
        state.pc++;
    }
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

void JP(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JP " << PC2_str(state) << '\n';
#endif

    if (state.flags.s == 0)
    {
        JMP(state);
    }
    else
    {
        state.pc += 3;
    }
}

void JPO(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JPO " << PC2_str(state) << '\n';
#endif

    if (!state.flags.p)
    {
        JMP(state);
    }
    else
    {
        state.pc += 3;
    }
}

void RM(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "RM " << PC2_str(state) << '\n';
#endif

    if (state.flags.s)
    {
        RET(state);
    }
    else
    {
        state.pc++;
    }
}
void JM(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "JM " << PC2_str(state) << '\n';
#endif

    if (state.flags.s)
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

void CZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CZ " << PC2_str(state) << '\n';
#endif

    if (state.flags.z)
    {
        CALL(state);
    }
    else
    {
        state.pc += 3;
    }
}

void CPE(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CPE " << PC2_str(state) << '\n';
#endif

    if (state.flags.p)
    {
        CALL(state);
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

void CNZ(State8080 &state)
{
#if ENABLE_LOGGING
    std::cout << "CNZ " << PC2_str(state) << '\n';
#endif

    if (!state.flags.z)
    {
        CALL(state);
    }
    else
    {
        state.pc += 3;
    }
}

void IN(State8080 &state, std::function<void(State8080 &)> mapping)
{
#if ENABLE_LOGGING
    std::cout << "IN " << PC1_str(state) << '\n';
#endif

    uint8_t port = state.memory[state.pc + 1];

    if (mapping)
    {
        mapping(state);
    }

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
    case 0x02: // STAX BC
        STAX(state, Utils::to_16(state.c, state.b), "BC");
        break;
    case 0x03: // INX BC
        INX(state, reinterpret_cast<uint16_t *>(&state.c), "BC");
        break;
    case 0x04: // INR B
        INR_R(state, &state.b, "B");
        break;
    case 0x05: // DCR B
        DCR_R(state, &state.b, "B");
        break;
    case 0x06: // MVI B
        MVI_R(state, &state.b, "B");
        break;
    case 0x07: // RLC
        RLC(state);
        break;
    case 0x08: // ???
        NOP(state);
        break;
    case 0x09: // DAD BC
        DAD(state, Utils::to_16(state.c, state.b), "BC");
        break;
    case 0x0A: // LDAX BC
        LDAX(state, Utils::to_16(state.c, state.b), "BC");
        break;
    case 0x0B: // DCX BC
        DCX(state, reinterpret_cast<uint16_t *>(&state.c), "BC");
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
    case 0x10: // ???
        NOP(state);
        break;
    case 0x11: // LXI DE
        LXI(state, reinterpret_cast<uint16_t *>(&state.e), "DE");
        break;
    case 0x13: // INX DE
        INX(state, reinterpret_cast<uint16_t *>(&state.e), "DE");
        break;
    case 0x16: // MVI D
        MVI_R(state, &state.d, "D");
        break;
    case 0x19: // DAD DE
        DAD(state, Utils::to_16(state.e, state.d), "DE");
        break;
    case 0x1A: // LDAX DE
        LDAX(state, Utils::to_16(state.e, state.d), "DE");
        break;
    case 0x1C: // INR E
        INR_R(state, &state.e, "E");
        break;
    case 0x1F: // RAR
        RAR(state);
        break;
    case 0x20: // ???
        NOP(state);
        break;
    case 0x21: // LXI HL
        LXI(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0x22: // SHLD
        SHLD(state);
        break;
    case 0x23: // INX HL
        INX(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0x24: // INR H
        INR_R(state, &state.h, "H");
        break;
    case 0x25: // DCR H
        DCR_R(state, &state.h, "H");
        break;
    case 0x26: // MVI H
        MVI_R(state, &state.h, "H");
        break;
    case 0x27: // DAA
        // TODO ???
        state.pc++;
        break;
    case 0x29: // DAD HL
        DAD(state, Utils::to_16(state.l, state.h), "HL");
        break;
    case 0x2A: // LHLD
        LHLD(state);
        break;
    case 0x2B: // DCX HL
        DCX(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0x2C: // INR L
        INR_R(state, &state.l, "L");
        break;
    case 0x2E: // MVI L
        MVI_R(state, &state.l, "L");
        break;
    case 0x2F:
        CMA(state);
        break;
    case 0x31: // LXI P
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
    case 0x38: // ???
        NOP(state);
        break;
    case 0x3A: // LDA adr
        LDA(state);
        break;
    case 0x3C: // INR A
        INR_R(state, &state.a, "A");
        break;
    case 0x3D: // DCR A
        DCR_R(state, &state.a, "A");
        break;
    case 0x3E: // MVI A
        MVI_R(state, &state.a, "A");
        break;
    case 0x41: // MOV B, C
        MOV_R_R(state, &state.b, state.c, "B", "C");
        break;
    case 0x42: // MOV B, D
        MOV_R_R(state, &state.b, state.d, "B", "D");
        break;
    case 0x43: // MOV B, E
        MOV_R_R(state, &state.b, state.e, "B", "E");
        break;
    case 0x44: // MOV B, H
        MOV_R_R(state, &state.b, state.h, "B", "H");
        break;
    case 0x45: // MOV B, L
        MOV_R_R(state, &state.b, state.l, "B", "L");
        break;
    case 0x46: // MOV B, (HL)
        MOV_R_M(state, &state.b, "B");
        break;
    case 0x47: // MOV B, A
        MOV_R_R(state, &state.b, state.a, "B", "A");
        break;
    case 0x48: // MOV C, B
        MOV_R_R(state, &state.c, state.b, "C", "B");
        break;
    case 0x49: // MOV C, C
        MOV_R_R(state, &state.c, state.c, "C", "C");
        break;
    case 0x4A: // MOV C, D
        MOV_R_R(state, &state.c, state.d, "C", "D");
        break;
    case 0x4B: // MOV C, E
        MOV_R_R(state, &state.c, state.e, "C", "E");
        break;
    case 0x4C: // MOV C, H
        MOV_R_R(state, &state.c, state.h, "C", "H");
        break;
    case 0x4D: // MOV C, L
        MOV_R_R(state, &state.c, state.l, "C", "L");
        break;
    case 0x4E: // MOV C, (HL)
        MOV_R_M(state, &state.c, "C");
        break;
    case 0x4F: // MOV C, A
        MOV_R_R(state, &state.c, state.a, "C", "A");
        break;
    case 0x54: // MOV D, H
        MOV_R_R(state, &state.d, state.h, "D", "H");
        break;
    case 0x56: // MOV D, (HL)
        MOV_R_M(state, &state.d, "D");
        break;
    case 0x57: // MOV D, A
        MOV_R_R(state, &state.d, state.a, "D", "A");
        break;
    case 0x58: // MOV E, B
        MOV_R_R(state, &state.e, state.b, "E", "B");
        break;
    case 0x59: // MOV E, C
        MOV_R_R(state, &state.e, state.c, "E", "C");
        break;
    case 0x5A: // MOV E, D
        MOV_R_R(state, &state.e, state.d, "E", "D");
        break;
    case 0x5B: // MOV E, E
        MOV_R_R(state, &state.e, state.e, "E", "E");
        break;
    case 0x5C: // MOV E, H
        MOV_R_R(state, &state.e, state.h, "E", "H");
        break;
    case 0x5D: // MOV E, L
        MOV_R_R(state, &state.e, state.l, "E", "L");
        break;
    case 0x5E: // MOV E, (HL)
        MOV_R_M(state, &state.e, "E");
        break;
    case 0x5F: // MOV E, A
        MOV_R_R(state, &state.e, state.a, "E", "A");
        break;
    case 0x60: // MOV H, B
        MOV_R_R(state, &state.h, state.b, "E", "B");
        break;
    case 0x61: // MOV H, C
        MOV_R_R(state, &state.h, state.c, "E", "C");
        break;
    case 0x62: // MOV H, D
        MOV_R_R(state, &state.h, state.d, "H", "D");
        break;
    case 0x63: // MOV H, E
        MOV_R_R(state, &state.h, state.e, "H", "E");
        break;
    case 0x64: // MOV H, H
        MOV_R_R(state, &state.h, state.h, "H", "H");
        break;
    case 0x65: // MOV H, L
        MOV_R_R(state, &state.h, state.l, "H", "L");
        break;
    case 0x66: // MOV H, (HL)
        MOV_R_M(state, &state.h, "H");
        break;
    case 0x67: // MOV H, A
        MOV_R_R(state, &state.h, state.a, "H", "A");
        break;
    case 0x68: // MOV L, B
        MOV_R_R(state, &state.l, state.b, "L", "B");
        break;
    case 0x69: // MOV L, C
        MOV_R_R(state, &state.l, state.c, "L", "C");
        break;
    case 0x6A: // MOV L, D
        MOV_R_R(state, &state.l, state.d, "L", "D");
        break;
    case 0x6B: // MOV L, E
        MOV_R_R(state, &state.l, state.e, "L", "E");
        break;
    case 0x6C: // MOV L, H
        MOV_R_R(state, &state.l, state.h, "L", "H");
        break;
    case 0x6D: // MOV L, L
        MOV_R_R(state, &state.l, state.l, "L", "L");
        break;
    case 0x6E: // MOV L, (HL)
        MOV_R_M(state, &state.l, "L");
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
    case 0x78: // MOV A, B
        MOV_R_R(state, &state.a, state.b, "A", "B");
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
    case 0x81: // ADD C
        ADD_R(state, state.c, "C");
        break;
    case 0x82: // ADD D
        ADD_R(state, state.d, "D");
        break;
    case 0x83: // ADD E
        ADD_R(state, state.e, "E");
        break;
    case 0x84: // ADD H
        ADD_R(state, state.h, "H");
        break;
    case 0x85: // ADD L
        ADD_R(state, state.l, "L");
        break;
    case 0x86: // ADD M
        ADD_M(state);
        break;
    case 0x87: // ADD A
        ADD_R(state, state.a, "A");
        break;
    case 0x88: // ADC B
        ADC_R(state, state.b, "B");
        break;
    case 0x89: // ADC C
        ADC_R(state, state.c, "C");
        break;
    case 0x8A: // ADC D
        ADC_R(state, state.d, "D");
        break;
    case 0x8B: // ADC E
        ADC_R(state, state.e, "E");
        break;
    case 0x8C: // ADC H
        ADC_R(state, state.h, "H");
        break;
    case 0x8D: // ADC L
        ADC_R(state, state.l, "L");
        break;
    case 0x8F: // ADC A
        ADC_R(state, state.a, "A");
        break;
    case 0x90: // SUB B
        SUB_R(state, state.b, "B");
        break;
    case 0x91: // SUB C
        SUB_R(state, state.c, "C");
        break;
    case 0x92: // SUB D
        SUB_R(state, state.d, "D");
        break;
    case 0x93: // SUB E
        SUB_R(state, state.e, "E");
        break;
    case 0x94: // SUB H
        SUB_R(state, state.h, "H");
        break;
    case 0x95: // SUB L
        SUB_R(state, state.l, "L");
        break;
    case 0x97: // SUB A
        SUB_R(state, state.a, "A");
        break;
    case 0xA0: // ANA B
        ANA_R(state, state.b, "B");
        break;
    case 0xA1: // ANA C
        ANA_R(state, state.c, "C");
        break;
    case 0xA2: // ANA D
        ANA_R(state, state.d, "D");
        break;
    case 0xA3: // ANA E
        ANA_R(state, state.e, "E");
        break;
    case 0xA4: // ANA H
        ANA_R(state, state.h, "H");
        break;
    case 0xA5: // ANA L
        ANA_R(state, state.l, "L");
        break;
    case 0xA6: // ANA L
        ANA_M(state);
        break;
    case 0xA7: // ANA A
        ANA_R(state, state.a, "A");
        break;
    case 0xA8: // XRA B
        XRA_R(state, state.b, "B");
        break;
    case 0xA9: // XRA C
        XRA_R(state, state.c, "C");
        break;
    case 0xAA: // XRA D
        XRA_R(state, state.d, "D");
        break;
    case 0xAB: // XRA E
        XRA_R(state, state.e, "E");
        break;
    case 0xAC: // XRA H
        XRA_R(state, state.h, "H");
        break;
    case 0xAD: // XRA L
        XRA_R(state, state.l, "L");
        break;
    case 0xAF: // XRA A
        XRA_R(state, state.a, "A");
        break;
    case 0xB0: // ORA B
        ORA_R(state, state.b, "B");
        break;
    case 0xB6: // ORA M
        ORA_M(state);
        break;
    case 0xB9: // CMP C
        CMP_R(state, state.c, "C");
        break;
    case 0xBB: // CMP E
        CMP_R(state, state.e, "E");
        break;
    case 0xC0: // RNZ
        RNZ(state);
        break;
    case 0xC1: // POP BC
        POP(state, reinterpret_cast<uint16_t *>(&state.c), "BC");
        break;
    case 0xC2: // JNZ
        JNZ(state);
        break;
    case 0xC3: // JMP
        JMP(state);
        break;
    case 0xC4: // CNZ
        CNZ(state);
        break;
    case 0xC5: // PUSH BC
        PUSH(state, state.c, state.b, "BC");
        break;
    case 0xC6: // ADI
        ADI(state);
        break;
    case 0xC7: // RST 0
        RST(state, 0, is_interruption);
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
    case 0xCC: // CZ
        CZ(state);
        break;
    case 0xCD: // CALL
        CALL(state);
        break;
    case 0xCF: // RST 1
        RST(state, 1, is_interruption);
        break;
    case 0xD0:
        RNC(state);
        break;
    case 0xD1: // POP DE
        POP(state, reinterpret_cast<uint16_t *>(&state.e), "DE");
        break;
    case 0xD3: // OUT
        OUT(state, out_mappings[state.memory[state.pc + 1]]);
        break;
    case 0xD4:
        CNC(state);
        break;
    case 0xD5: // PUSH DE
        PUSH(state, state.e, state.d, "DE");
        break;
    case 0xD6: // SUI
        SUI(state);
        break;
    case 0xD7: // RST 2
        RST(state, 2, is_interruption);
        break;
    case 0xD8: // RC
        RC(state);
        break;
    case 0xDA: // JC
        JC(state);
        break;
    case 0xDB: // IN
        IN(state, in_mappings[state.memory[state.pc + 1]]);
        break;
    case 0xDD: // ???
        NOP(state);
        break;
    case 0xE1: // POP HL
        POP(state, reinterpret_cast<uint16_t *>(&state.l), "HL");
        break;
    case 0xE2: // JPO
        JPO(state);
        break;
    case 0xE3: // XTHL
        XTHL(state);
        break;
    case 0xE5: // PUSH HL
        PUSH(state, state.l, state.h, "HL");
        break;
    case 0xE6: // ANI
        ANI(state);
        break;
    case 0xE9: // PCHL
        PCHL(state);
        break;
    case 0xEB: // XCHG
        XCHG(state);
        break;
    case 0xEC: // CPE
        CPE(state);
        break;
    case 0xF1: // POP_PSW
        POP_PSW(state);
        break;
    case 0xF2: // JP
        JP(state);
        break;
    case 0xF3: // DI
        state.interruptions_enabled = false;
        state.pc++;
        break;
    case 0xF5:
        PUSH_PSW(state);
        break;
    case 0xF6:
        ORI(state);
        break;
    case 0xF8: // RM
        RM(state);
        break;
    case 0xFA: // JM
        JM(state);
        break;
    case 0xFB: // EI
        state.interruptions_enabled = true;
        state.pc += 1;
        break;
    case 0xFE: // CPI
        CPI(state);
        break;
    case 0xFF: // RST 7
        RST(state, 7, is_interruption);
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

void Emulator8080::connect_in_port(uint8_t port, std::function<void(State8080 &state)> handler)
{
    in_mappings[port] = handler;
}

void Emulator8080::connect_out_port(uint8_t port, std::function<void(State8080 &state)> handler)
{
    out_mappings[port] = handler;
}