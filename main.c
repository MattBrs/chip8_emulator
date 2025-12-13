#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "stack.h"

int main() {
  stack_init(&functions_stack, 128);

  SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer("Chip-8 Emulator", 640, 320, SDL_WINDOW_OPENGL,
                              &window, &renderer);

  if (window == NULL || renderer == NULL) {
    printf("could not init window or renderer!\n");
    close_sdl(window, renderer);
    return 1;
  }

  SDL_SetRenderLogicalPresentation(renderer, 64, 32,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    render(renderer);
    SDL_Delay(1000 / 60);
  }

  close_sdl(window, renderer);

  printf("bye bye!\n");
  return 0;
}

void render(SDL_Renderer *renderer) {
  SDL_Surface *screen_surface =
      SDL_CreateSurface(64, 32, SDL_PIXELFORMAT_RGBA8888);
  for (int i = 0; i < 64; ++i) {
    for (int j = 0; j < 32; ++j) {
      set_pixel_color(screen_surface, i, j, screen_state[i][j]);
    }
  }

  SDL_Texture *screen_texture =
      SDL_CreateTextureFromSurface(renderer, screen_surface);

  SDL_Rect texture_rect;
  texture_rect.x = 0;
  texture_rect.y = 0;
  texture_rect.w = 64;
  texture_rect.h = 32;

  SDL_RenderClear(renderer);
  SDL_RenderTexture(renderer, screen_texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void close_sdl(SDL_Window *window, SDL_Renderer *renderer) {
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }

  if (window) {
    SDL_DestroyWindow(window);
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  SDL_Quit();
}

void set_pixel_color(SDL_Surface *surface, const int x, const int y,
                     const uint32_t color) {
  if (surface == NULL) {
    return;
  }

  Uint8 *pixel = (Uint8 *)surface->pixels;
  pixel += (y * surface->pitch) + (x * sizeof(uint32_t));
  *((uint32_t *)pixel) = color;
}

void init_emulator() {
  memcpy(memory + 0x050, fonts, 80); // copy fonts into mem
  program_counter = 0x200;
}

void execute_cycle() {
  // run fetch, decode and execute
}
