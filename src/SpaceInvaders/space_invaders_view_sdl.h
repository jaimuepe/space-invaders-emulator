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

    virtual void render(const uint8_t *video_memory) override;

    virtual bool should_quit() const override;

    virtual ~SpaceInvadersViewSDL();

private:
    bool exit_requested{};

    SDL_Window *window{};

    SDL_Renderer *renderer{};

    SDL_Surface *surface{};
};