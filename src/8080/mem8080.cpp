#include "mem8080.h"

#include "../reader.h"

void Mem8080::load()
{
    Reader::copy_at("invaders.h", memory + 0x0000);
    Reader::copy_at("invaders.g", memory + 0x0800);
    Reader::copy_at("invaders.f", memory + 0x1000);
    Reader::copy_at("invaders.e", memory + 0x1800);
}

uint8_t &Mem8080::operator[](int idx)
{
    return memory[idx];
}

uint8_t Mem8080::operator[](int idx) const
{
    return memory[idx];
}