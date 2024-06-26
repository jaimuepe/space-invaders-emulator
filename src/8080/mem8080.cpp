#include "mem8080.h"

#include "../reader.h"

#include <iostream>

void Mem8080::init()
{
#if CPU_DIAG
    Reader::copy_at("res/TST8080.COM", memory + 0x100);
#else
    Reader::copy_at("res/invaders.h", memory + 0x0000);
    Reader::copy_at("res/invaders.g", memory + 0x0800);
    Reader::copy_at("res/invaders.f", memory + 0x1000);
    Reader::copy_at("res/invaders.e", memory + 0x1800);
#endif
}

const uint8_t &Mem8080::operator[](uint16_t pos) const
{
    return memory[pos];
}

uint8_t &Mem8080::operator[](uint16_t pos)
{
    return memory[pos];
}