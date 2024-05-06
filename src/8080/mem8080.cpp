#include "mem8080.h"

#include "../reader.h"

#include <iostream>

void Mem8080::init()
{
    Reader::copy_at("invaders.h", memory + 0x0000);
    Reader::copy_at("invaders.g", memory + 0x0800);
    Reader::copy_at("invaders.f", memory + 0x1000);
    Reader::copy_at("invaders.e", memory + 0x1800);
}

const uint8_t &Mem8080::operator[](uint16_t pos) const
{
    return memory[pos];
}

uint8_t &Mem8080::operator[](uint16_t pos)
{
    return memory[pos];
}