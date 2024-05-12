#pragma once

#include "../emulator8080.h"

class CPUDiag
{
public:
    void init();

    void run();
    
private:
    Emulator8080 emulator{};
};