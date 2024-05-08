#include "space_invaders_view_sdl.h"

#include "SDL3/SDL.h"

#include <iostream>

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 224;

SpaceInvadersViewSDL::SpaceInvadersViewSDL() {}

void SpaceInvadersViewSDL::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "could not initialize sdl2: " << SDL_GetError() << '\n';
        exit(1);
    }

    window = SDL_CreateWindow(
        "space_invaders",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);

    if (window == nullptr)
    {
        std::cerr << "could not create window: " << SDL_GetError() << '\n';
        exit(1);
    }

    surface = SDL_GetWindowSurface(window);
}

bool SpaceInvadersViewSDL::render()
{
    SDL_Event eventData;

    bool keepGoing = true;

    while (SDL_PollEvent(&eventData))
    {
        switch (eventData.type)
        {
        case SDL_EVENT_QUIT:
            keepGoing = false;
            break;
        }
    }

    return keepGoing;
}

SpaceInvadersViewSDL::~SpaceInvadersViewSDL()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}