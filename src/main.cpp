#include "SpaceInvaders/space_invaders.h"
#include "8080/CPUDiag/cpu_diag.h"

int main(int argc, char *argv[])
{
#if CPU_DIAG

    CPUDiag diag{};
    diag.init();

    diag.run();

#else
    SpaceInvaders invaders{};
    invaders.init();

    invaders.run();
#endif

    return 0;
}