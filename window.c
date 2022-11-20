#include "window.h"

#include <SDL2/SDL.h>

static SDL_Window* g_window;

static bool
sdl_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[ERROR] Could not initialize sdl2: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool
window_create(uint32_t width, uint32_t height)
{
    if (!sdl_init()) {
        return false;
    }
    
    g_window = SDL_CreateWindow("Image Loader",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                width, height, 0);
    
    if (g_window == NULL) {
        fprintf(stderr, "[ERROR] Could not create window: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

static void
draw_image(SDL_Surface* screen_surface, uint32_t* pixels,
           uint32_t width, uint32_t height)
{
    SDL_Surface* img_surface =
        SDL_CreateRGBSurfaceFrom(pixels,
                                 width, height, 32, 4 * width,
                                 0xFF << 24, 0xFF << 16, 0xFF << 8, 0xFF << 0);
    uint32_t screen_width = screen_surface->clip_rect.w;
    uint32_t screen_height = screen_surface->clip_rect.h;
    int32_t margin_x = ((int32_t)screen_width - (int32_t)width) / 2;
    int32_t margin_y = ((int32_t)screen_height - (int32_t)height) / 2;

    SDL_Rect rect = {
        .x = margin_x, .y = margin_y,
        .w = screen_width, .h = screen_height
    };
    
    SDL_BlitSurface(img_surface, &img_surface->clip_rect,
                    screen_surface, &rect);
}

static void
draw_transparency_background(SDL_Surface* screen_surface)
{    
    uint32_t grey_pixel  = 0xBFBFBFFF;
    uint32_t white_pixel = 0xFFFFFFFF;
    uint32_t cell_size = 50;

    uint32_t width   = screen_surface->w;
    uint32_t height  = screen_surface->h;
    uint32_t* pixels = malloc(width * height * sizeof(uint32_t));

    bool start_with_white = true;
    for (uint32_t y = 0; y < height; ++y) {
        if (y % cell_size == 0) {
            start_with_white = !start_with_white;
        }
        bool white = start_with_white;   
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t i = x + y * width;
            if (x % cell_size == 0) {
                white = !white;
            }
            pixels[i] = white ? white_pixel : grey_pixel;
        }
    }
    
    SDL_Surface* background_surface =
        SDL_CreateRGBSurfaceFrom(pixels,
                                 width, height, 32, 4 * width,
                                 0xFF << 24, 0xFF << 16, 0xFF << 8, 0xFF << 0);

    SDL_BlitSurface(background_surface, &background_surface->clip_rect,
                    screen_surface, &screen_surface->clip_rect);

    free(pixels);
}

bool
window_update(uint32_t* pixels, uint32_t width, uint32_t height)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: {
                return false;
            } break;
            /* case SDL_WINDOWEVENT: { */
                /* switch (e.window.event) { */
                    /* case SDL_WINDOWEVENT_RESIZED: { */
                    /*     uint32_t resized_w = e.window.data1; */
                    /*     uint32_t resized_h = e.window.data2; */
                    /*     if ((uint32_t)screen_surface->w != resized_w) { */
                    /*         float aspect_ratio = height / (float)width; */
                    /*         resized_h = resized_w * aspect_ratio; */
                    /*     } else if ((uint32_t)screen_surface->h != resized_h) { */
                    /*         float aspect_ratio = width / (float)height; */
                    /*         resized_w = resized_h * aspect_ratio; */
                    /*     } */
                    /*     SDL_SetWindowSize(g_window, resized_w, resized_h); */
                    /*     screen_surface = SDL_GetWindowSurface(g_window); */
                    /* } break; */
                /* } */
            /* } break; */
        }
    }

    SDL_Surface* screen_surface = SDL_GetWindowSurface(g_window); 
    draw_transparency_background(screen_surface);
    draw_image(screen_surface, pixels, width, height);
    SDL_UpdateWindowSurface(g_window);

    return true;
}

void
window_destroy(void)
{
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}
