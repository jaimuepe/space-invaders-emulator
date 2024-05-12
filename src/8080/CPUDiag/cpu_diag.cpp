#include "cpu_diag.h"

void CPUDiag::init()
{
    emulator.init();
}

void CPUDiag::run()
{
    for (;;)
    {
        emulator.step();
    }
}