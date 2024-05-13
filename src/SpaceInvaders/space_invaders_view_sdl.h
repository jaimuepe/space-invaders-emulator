#pragma once

#include "space_invaders_view.h"

#include <cstdint>
#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

constexpr int SCREEN_WIDTH = 224;
constexpr int SCREEN_HEIGHT = 256;

class SpaceInvadersViewSDL : public SpaceInvadersView
{
public:
    SpaceInvadersViewSDL();

    virtual void init() override;

    virtual void poll_events(uint8_t inputs[]) override;

    virtual void update_screen_buffer(const uint8_t *video_memory) override;

    virtual void render() override;

    virtual bool should_quit() const override;

    virtual ~SpaceInvadersViewSDL();

private:
    bool exit_requested{};

    uint8_t screen_buffer[SCREEN_HEIGHT][SCREEN_WIDTH][4]{0};

    SDL_Window *window{};

    SDL_Renderer *renderer{};

    SDL_Texture *texture{};
};