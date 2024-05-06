#pragma once

#include <cstdint>
#include <string>

class Reader
{
public:
    static void copy_at(const std::string &fileName, uint8_t *position);
};