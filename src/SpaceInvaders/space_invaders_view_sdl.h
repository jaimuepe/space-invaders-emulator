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

    virtual bool render(uint8_t *video_memory) override;

    virtual ~SpaceInvadersViewSDL();

private:
    SDL_Window *window{};

    SDL_Renderer *renderer{};

    SDL_Surface *surface{};
};