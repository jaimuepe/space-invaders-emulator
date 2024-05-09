#pragma once

#include "space_invaders_view.h"

#include <cstdint>
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;

class SpaceInvadersViewSDL : public SpaceInvadersView
{
public:
    SpaceInvadersViewSDL();

    virtual void init() override;

    virtual void poll_events() override;

    virtual bool render(uint8_t *video_memory) override;

    virtual bool should_quit() const override;

    virtual ~SpaceInvadersViewSDL();

private:
    bool exit_requested{};

    float accumulator{0.0f};

    uint64_t last_performance_counter{0};

    SDL_Window *window{};

    SDL_Renderer *renderer{};

    SDL_Surface *surface{};
};