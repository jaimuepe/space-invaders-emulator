#pragma once

#include <cstdint>

class SpaceInvadersView
{
public:
    virtual void init() = 0;

    virtual bool render(uint8_t *video_memory) = 0;

    virtual ~SpaceInvadersView() {}
};