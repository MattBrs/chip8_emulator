#include <SDL3/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// stack BEGIN
const int MAX_ALLOWED_STACK_SIZE = 256;
typedef struct stack {
  int head;
  int max_size;
  int16_t data[MAX_ALLOWED_STACK_SIZE];
} Stack;

bool stack_is_empty(Stack *s) { return s->head == -1; }
bool stack_is_full(Stack *s) { return s->head == s->max_size; }

void stack_init(Stack *s, int max_size) {
  if (max_size > MAX_ALLOWED_STACK_SIZE) {
    printf("allocating more stack space than allowed is prohibited");
    exit(1);
  }

  s->max_size = max_size;
  s->head = -1;
}

void stack_push(Stack *s, int16_t val) {
  if (stack_is_full(s)) {
    printf("stack overflow!\n");
    exit(1);
  }

  ++s->head;
  s->data[s->head] = val;
}

int16_t stack_pop(Stack *s) {
  if (stack_is_empty(s)) {
    printf("cannot pop empty stack!\n");
    exit(1);
  }

  int val = s->data[s->head];
  --s->head;

  return val;
}

void stack_print(Stack *s) {
  for (int i = 0; i <= s->head; ++i) {
    printf("%d ", s->data[i]);
  }

  printf("\n");
}

int16_t stack_peek(Stack *s) {
  if (stack_is_empty(s)) {
    printf("cannot peek empty stack\n");
    exit(1);
  }

  return s->data[s->head];
}
// stack END

void close_sdl(SDL_Window *window, SDL_Renderer *renderer);
void render(SDL_Renderer *renderer);

int main() {
  static int memory[4096];
  static int progra_counter;
  static int16_t i;
  static int8_t v[16];
  static Stack functions_stack;

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
