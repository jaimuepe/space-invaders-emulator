#pragma once

#include "space_invaders_view.h"

#include <memory>

struct SDL_Window;
struct SDL_Surface;

class SpaceInvadersViewSDL : public SpaceInvadersView
{
public:
    SpaceInvadersViewSDL();

    virtual void init() override;

    virtual bool render() override;

    virtual ~SpaceInvadersViewSDL();

private:
    SDL_Window *window;

    SDL_Surface *surface;
};