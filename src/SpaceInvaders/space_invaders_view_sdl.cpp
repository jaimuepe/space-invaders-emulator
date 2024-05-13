#include "space_invaders_view_sdl.h"

#include "SDL.h"

#include <iostream>

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

    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr)
    {
        std::cerr << "could not create renderer: " << SDL_GetError() << '\n';
        exit(1);
    }

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 0);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (texture == nullptr)
    {
        std::cerr << "could not create texture: " << SDL_GetError() << '\n';
        exit(1);
    }
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
                inputs[1] |= 0x01;
            }
            break;
        case SDL_KEYUP:
            break;
        case SDL_QUIT:
            exit_requested = true;
            break;
        }
    }
}

void SpaceInvadersViewSDL::update_screen_buffer(const uint8_t *video_memory)
{
    for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH / 8; i++)
    {
        const int y = i * 8 / 256;
        const int base_x = (i * 8) % 256;

        const uint8_t cur_byte = video_memory[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            int px = base_x + bit;
            int py = y;

            const bool is_pixel_lit = (cur_byte >> bit) & 1;

            uint8_t r = 0, g = 0, b = 0;

            if (is_pixel_lit)
            {
                r = 255;
                g = 255;
                b = 255;
            }

            // space invaders' screen is rotated 90 degrees anti-clockwise
            // so we invert the coordinates:
            const int temp_x = px;
            px = py;
            py = -temp_x + SCREEN_HEIGHT - 1;

            screen_buffer[py][px][0] = r;
            screen_buffer[py][px][1] = g;
            screen_buffer[py][px][2] = b;
        }
    }

    void *texture_pixels;
    int pitch;

    if (SDL_LockTexture(texture, NULL, &texture_pixels, &pitch) != 0)
    {
        std::cerr << "unable to lock texture: " << SDL_GetError() << '\n';
    }
    else
    {
        memcpy(texture_pixels, screen_buffer, pitch * SCREEN_HEIGHT);
        SDL_UnlockTexture(texture);
    }
}

void SpaceInvadersViewSDL::render()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

bool SpaceInvadersViewSDL::should_quit() const
{
    return exit_requested;
}

SpaceInvadersViewSDL::~SpaceInvadersViewSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}