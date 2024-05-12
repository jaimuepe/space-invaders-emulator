#include "space_invaders_view_sdl.h"

#include "SDL.h"

#include <iostream>

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 224;

SpaceInvadersViewSDL::SpaceInvadersViewSDL() {}

void SpaceInvadersViewSDL::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cerr << "could not initialize sdl2: " << SDL_GetError() << '\n';
        exit(1);
    }

    window = SDL_CreateWindow(
        "Space Invaders",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        3 * SCREEN_WIDTH,
        3 * SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == nullptr)
    {
        std::cerr << "could not create window: " << SDL_GetError() << '\n';
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == nullptr)
    {
        std::cerr << "could not create renderer: " << SDL_GetError() << '\n';
        exit(1);
    }

    // SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    // SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0);

    surface = SDL_CreateRGBSurfaceWithFormat(0,
                                             SCREEN_WIDTH,
                                             SCREEN_HEIGHT,
                                             1,
                                             SDL_PIXELFORMAT_INDEX8);

    if (surface == nullptr)
    {
        std::cerr << "could not create surface: " << SDL_GetError() << '\n';
        exit(1);
    }

    SDL_Color colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetPaletteColors(surface->format->palette, colors, 0, 2);
}

void SpaceInvadersViewSDL::poll_events(uint8_t inputs[])
{
    SDL_Event eventData;
    while (SDL_PollEvent(&eventData))
    {
        switch (eventData.type)
        {
        case SDL_KEYDOWN:
            if (eventData.key.keysym.sym == SDL_KeyCode::SDLK_c)
            {
                inputs[1] | 0x01;
            }
            break;
        case SDL_KEYUP:
            if (eventData.key.keysym.sym == SDL_KeyCode::SDLK_c)
            {
                inputs[1] & ~0x01;
            }
            break;
        case SDL_QUIT:
            exit_requested = true;
            break;
        }
    }
}

void SpaceInvadersViewSDL::render(const uint8_t *video_memory)
{
    if (SDL_MUSTLOCK(surface))
    {
        SDL_LockSurface(surface);
    }

    uint8_t *pixels = (uint8_t *)surface->pixels;

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        int row = i / SCREEN_WIDTH;
        int col = i % SCREEN_WIDTH;

        int p = i % 8;
        uint8_t bit = (video_memory[i / 8] >> (7 - p)) & 0x01;

        uint8_t *target_pixel = pixels + row * surface->pitch + col * surface->format->BytesPerPixel;

        *target_pixel = bit;
    }

    if (SDL_MUSTLOCK(surface))
    {
        SDL_UnlockSurface(surface);
    }

    SDL_RenderClear(renderer);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(screen_texture);
}

bool SpaceInvadersViewSDL::should_quit() const
{
    return exit_requested;
}

SpaceInvadersViewSDL::~SpaceInvadersViewSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}