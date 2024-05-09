#pragma once

#include <cstdint>

class SpaceInvadersView
{
public:
    virtual void init() = 0;

    virtual void poll_events() = 0;
    
    virtual void render(const uint8_t *video_memory) = 0;

    virtual bool should_quit() const = 0;

    virtual ~SpaceInvadersView() {}
};