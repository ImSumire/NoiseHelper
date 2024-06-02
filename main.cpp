#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include <string>
#include <map>
#include <chrono>

// #include "libs/imgui.h"
// #include "libs/imgui_impl_sdl.h"
// #include "libs/imgui_impl_opengl3.h"
#include "libs/noise.hpp"

// Parameters structure
struct Parameters
{
    int canvaSize;
    int tileSize;
    float amplitude;
    int octaves;
    float persistence;
    float lacunarity;
};

void drawText(const char *text, int x, int y, SDL_Color color, SDL_Renderer *renderer, TTF_Font *ttf) {
            SDL_Surface *surface = TTF_RenderText_Blended(ttf, text, color);
            if (!surface) {
                std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
                exit(EXIT_FAILURE);
            }
            SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            SDL_Rect dest = { x, y, surface->w, surface->h };
            SDL_RenderCopy(renderer, text_texture, NULL, &dest);
            SDL_DestroyTexture(text_texture);
        }

// Function to initialize SDL
bool initSDL(SDL_Window **window, SDL_Renderer **renderer, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("Simplex Noise Helper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
    if (*window == NULL)
    {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL)
    {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(*window);
        return false;
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        return false;
    }

    return true;
}

// Function to render text
SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface)
    {
        std::cerr << "Text render error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char *args[])
{
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    if (!initSDL(&window, &renderer, WIDTH, HEIGHT))
    {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("res/Product Sans Bold.ttf", 24);
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    Parameters parms = {110, 6, 0.1f, 1, 0.5f, 2.0f};
    int cx = 0, cy = 0;

    SDL_Event e;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 24, 25, 38, 255);
        SDL_RenderClear(renderer);

        // Generate and render noise
        SimplexNoise sn;

        for (int y = 0; y < parms.canvaSize; y++)
        {
            for (int x = 0; x < parms.canvaSize; x++)
            {
                double n = sn.signedFBM((x + cx) * parms.amplitude, (y + cy) * parms.amplitude, parms.octaves, parms.lacunarity, parms.persistence);
                n = (n + 1.0) * 127.0;
                boxRGBA(renderer, x * parms.tileSize + 400, y * parms.tileSize + 25, (x + 1) * parms.tileSize + 400, (y + 1) * parms.tileSize + 25, n, n, n, 255);
            }
        }

        // Render GUI (labels, sliders)
        SDL_Color textColor = {255, 255, 255, 255};
        drawText("Fps: ...", 25, 60, textColor, renderer, font);
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
