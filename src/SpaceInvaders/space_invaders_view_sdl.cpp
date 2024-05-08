#include "space_invaders_view_sdl.h"

#include "SDL2/SDL.h"

#include <iostream>

constexpr int SCREEN_WIDTH = 224;
constexpr int SCREEN_HEIGHT = 256;

SpaceInvadersViewSDL::SpaceInvadersViewSDL() {}

void SpaceInvadersViewSDL::init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_RenderSetLogicalSize(renderer, 3 * SCREEN_WIDTH, 3 * SCREEN_HEIGHT);

    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    surface = SDL_CreateRGBSurfaceWithFormat(0,
                                             SCREEN_WIDTH,
                                             SCREEN_HEIGHT,
                                             1,
                                             SDL_PIXELFORMAT_INDEX8);

    SDL_Color colors[2] = {{0, 0, 0, 255}, {255, 255, 255, 255}};
    SDL_SetPaletteColors(surface->format->palette, colors, 0, 2);
}

#define RGB_ON 0xFFFFFFFFL
#define RGB_OFF 0x00000000L

bool SpaceInvadersViewSDL::render(uint8_t *video_memory)
{
    SDL_Event eventData;

    bool keepGoing = true;

    while (SDL_PollEvent(&eventData))
    {
        switch (eventData.type)
        {
        case SDL_QUIT:
            keepGoing = false;
            break;
        }
    }

    uint8_t *pixels = (uint8_t *)surface->pixels;

    // video buffer is rotated -90º
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
        for (int j = 0; j < SCREEN_HEIGHT; j++)
        {
            int idx = i * SCREEN_HEIGHT + j;
            int p = idx % 8;

            uint8_t bit = (video_memory[idx / 8] >> (7 - p)) & 0x01;

            uint8_t *target_pixel = pixels + j * surface->pitch + i * surface->format->BytesPerPixel;

            *target_pixel = bit;
        }
    }

    SDL_RenderClear(renderer);
    SDL_Texture *screen_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(screen_texture);

    return keepGoing;
}

SpaceInvadersViewSDL::~SpaceInvadersViewSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}