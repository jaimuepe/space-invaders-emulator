#pragma once

#include <cstdint>

class SpaceInvadersView
{
public:
    virtual void init() = 0;

    virtual void update_screen_buffer(const uint8_t *video_memory) = 0;

    virtual void poll_events(uint8_t inputs[]) = 0;

    virtual void render() = 0;

    virtual bool should_quit() const = 0;

    virtual ~SpaceInvadersView() {}
};