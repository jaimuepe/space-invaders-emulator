#include "SpaceInvaders/space_invaders.h"

int main(int argc, char *argv[])
{
    SpaceInvaders invaders{};
    invaders.init();

    invaders.run();

    return 0;
}