#include <SDL3/SDL.h>
#include <stdio.h>

void close_sdl(SDL_Window *window, SDL_Renderer *renderer);
void render(SDL_Renderer *renderer);

int main() {
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
  SDL_SetRenderDrawColor(renderer, 12, 34, 12, 255);
  SDL_RenderClear(renderer);
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
