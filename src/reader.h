#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

void copy_at(const std::string &fileName, uint8_t *position)
{
    std::filesystem::path filePath{fileName};
    const auto size = std::filesystem::file_size(filePath);

    char *buffer = reinterpret_cast<char *>(position);

    std::ifstream file(fileName, std::ios::binary | std::ios::in);

    if (!file.read(buffer, size))
    {
        std::cerr << "could not read file " << fileName << '\n';
        exit(1);
    }
}