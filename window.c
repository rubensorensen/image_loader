#include "window.h"

#include <SDL2/SDL.h>

static SDL_Window* g_window;

static float zoom = 1.0f;
static int32_t x_offset;
static int32_t y_offset;

static bool lmb_down;

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

    float max_size_rel_to_screen = 0.75f;
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    uint32_t max_width = dm.w * max_size_rel_to_screen;
    uint32_t max_height = dm.h * max_size_rel_to_screen;
    if (width > max_width) width = max_width;
    if (height > max_height) height = max_height;;
    
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
    
    uint32_t center_x = screen_surface->clip_rect.w / 2;
    uint32_t center_y = screen_surface->clip_rect.h / 2;
    int32_t x = center_x - (width / 2) * zoom;
    int32_t y = center_y - (height / 2) * zoom;

    SDL_Rect rect = {
        .x = x + x_offset, .y = y + y_offset,
        .w = width * zoom, .h = height * zoom
    };

    SDL_BlitScaled(img_surface, &img_surface->clip_rect,
                   screen_surface, &rect);
}

static void
draw_transparency_background(SDL_Surface* screen_surface)
{    
    uint32_t grey_pixel  = 0x9b9b99FF;
    uint32_t white_pixel = 0xbdbcbaFF;
    uint32_t cell_size = 10;

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
            case SDL_MOUSEBUTTONDOWN: {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    lmb_down = true;
                }
            } break;
            case SDL_MOUSEBUTTONUP: {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    lmb_down = false;
                }
            } break;
            case SDL_MOUSEMOTION: {
                if (lmb_down) {
                    x_offset += e.motion.xrel;
                    y_offset += e.motion.yrel;
                }
            } break;
            case SDL_MOUSEWHEEL: {
                zoom += e.wheel.y * 0.1f;
                if (zoom < 0.05f) {
                    zoom = 0.05f;
                }
            } break;
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
